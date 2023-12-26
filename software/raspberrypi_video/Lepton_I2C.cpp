#include "Lepton_I2C.h"

#include "leptonSDKEmb32PUB/LEPTON_SDK.h"
#include "leptonSDKEmb32PUB/LEPTON_SYS.h"
#include "leptonSDKEmb32PUB/LEPTON_OEM.h"
#include "leptonSDKEmb32PUB/LEPTON_Types.h"
#include "leptonSDKEmb32PUB/LEPTON_AGC.h"		//adding the AGC with the same format as the SDK includes above
#include <string>
#include <iostream>
#include <stdio.h>
bool _connected;

LEP_CAMERA_PORT_DESC_T _port;

int lepton_connect() {
	LEP_OpenPort(1, LEP_CCI_TWI, 400, &_port);
	_connected = true;
	return 0;
}

void lepton_perform_ffc() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RunSysFFCNormalization(&_port);
}

//presumably more commands could go here if desired

/*************************************************************************************************************/
/** AUTOMATIC GAIN CONTROL COMMANDS                                                                                            **/
/*************************************************************************************************************/


/* Enable AGC
*/
void lepton_enable_agc() {
    if(!_connected) {
        lepton_connect();
    }
	LEP_RESULT result = LEP_SetAgcEnableState(&_port, LEP_AGC_ENABLE);

	if(result != LEP_OK) {
        std::cerr << "Error setting AGC. Error code: " << result << std::endl;
    } else {
        std::cout << "AGC enabled" << std::endl;
    }
}
// Disable AGC
void lepton_disable_agc() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_SetAgcEnableState(&_port, LEP_AGC_DISABLE);

	if(result != LEP_OK) {
		std::cerr << "Error setting AGC. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC disabled" << std::endl;
	}
}
std::string check_agc_state() {
    if(!_connected) {
        lepton_connect();
    }
    LEP_AGC_ENABLE_E  agcEnableState;
    LEP_GetAgcEnableState(&_port, & agcEnableState);
    if( agcEnableState == LEP_AGC_ENABLE) {
        return "AGC is enabled\n";
    } else {
        return "AGC is disabled\n";
    }
}

/* AGC Histogram Equalization Policy */
void lepton_set_policy_heq() {
    if(!_connected) {
        lepton_connect();
    }
    LEP_SetAgcPolicy(&_port, LEP_AGC_HEQ);
	LEP_RESULT result = LEP_SetAgcPolicy(&_port, LEP_AGC_HEQ);

	if(result != LEP_OK) {
        std::cerr << "Error setting AGC HEQ. Error code: " << result << std::endl;
    } else {
        std::cout << "AGC HEQ set successfully" << std::endl;
    }
}

/* AGC Linear Policy */
void lepton_set_policy_linear() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_SetAgcPolicy(&_port, LEP_AGC_LINEAR);
	LEP_RESULT result = LEP_SetAgcPolicy(&_port, LEP_AGC_LINEAR);

	if(result != LEP_OK) {
		std::cerr << "Error setting AGC Linear. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC Linear set successfully" << std::endl;
	}
}

// AGC ROI
void lepton_set_agc_roi(uint16_t startCol, uint16_t startRow, uint16_t endCol, uint16_t endRow) {
    if(!_connected) {
        lepton_connect();
    }
    // Define the ROI
    LEP_AGC_ROI_T agcROI;
    agcROI.startCol = startCol;
    agcROI.startRow = startRow;
    agcROI.endCol = endCol;
    agcROI.endRow = endRow;
	LEP_RESULT result = LEP_SetAgcROI(&_port, agcROI);
	
	if(result != LEP_OK) {
        std::cerr << "Error setting AGC ROI. Error code: " << result << std::endl;
    } else {
        std::cout << "AGC ROI set successfully" << std::endl;
    }
}
void lepton_get_agc_roi() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_AGC_ROI_T currentROI;
	LEP_RESULT result = LEP_GetAgcROI(&_port, &currentROI);

	if(result != LEP_OK) {
		std::cerr << "Error getting AGC ROI. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AGC ROI: " << std::endl;
		std::cout << "Start Column: " << currentROI.startCol << std::endl;
		std::cout << "Start Row: " << currentROI.startRow << std::endl;
		std::cout << "End Column: " << currentROI.endCol << std::endl;
		std::cout << "End Row: " << currentROI.endRow << std::endl;
	}
}

// HEQ Dampening 
void SetHeqDampening(uint16_t dampening) {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_SetAgcHeqDampingFactor(&_port, dampening);
	if(result != LEP_OK) {
		std::cerr << "Error setting AGC Dampening. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC Dampening set successfully" << std::endl;
	}
}
void GetHeqDampening() {
	if(!_connected) {
		lepton_connect();
	}
	uint16_t currentDampening;
	LEP_RESULT result = LEP_GetAgcHeqDampingFactor(&_port, &currentDampening);

	if(result != LEP_OK) {
		std::cerr << "Error getting AGC Dampening. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AGC Dampening: " << currentDampening << std::endl;
	}
}

// Max AGC Gain
void lepton_max_gain(uint16_t maxGain) {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_SetAgcHeqMaxGain(&_port, maxGain);
	if(result != LEP_OK) {
		std::cerr << "Error setting AGC Max Gain. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC Max Gain set successfully" << std::endl;
	}
}
void lepton_get_agc_max_gain() {
	if(!_connected) {
		lepton_connect();
	}
	uint16_t currentMaxGain;
	LEP_RESULT result = LEP_GetAgcHeqMaxGain(&_port, &currentMaxGain);

	if(result != LEP_OK) {
		std::cerr << "Error getting AGC Max Gain. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AGC Max Gain: " << currentMaxGain << std::endl;
	}
}

// HEQ Clip Limit High
void SetHeqClipHigh(uint16_t clipLimitHigh) {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_SetAgcHeqClipLimitHigh(&_port, clipLimitHigh);
	if(result != LEP_OK) {
		std::cerr << "Error setting AGC Clip Limit High. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC Clip Limit High set successfully" << std::endl;
	}
}
void GetHeqClipHigh() {
	if(!_connected) {
		lepton_connect();
	}
	uint16_t currentClipLimitHigh;
	LEP_RESULT result = LEP_GetAgcHeqClipLimitHigh(&_port, &currentClipLimitHigh);

	if(result != LEP_OK) {
		std::cerr << "Error getting AGC Clip Limit High. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AGC Clip Limit High: " << currentClipLimitHigh << std::endl;
	}
}

// HEQ Clip Limit Low
void SetHeqClipLow(uint16_t clipLimitLow) {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_SetAgcHeqClipLimitLow(&_port, clipLimitLow);
	if(result != LEP_OK) {
		std::cerr << "Error setting AGC Clip Limit Low. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC Clip Limit Low set successfully" << std::endl;
	}
}
void GetHeqClipLow() {
	if(!_connected) {
		lepton_connect();
	}
	uint16_t currentClipLimitLow;
	LEP_RESULT result = LEP_GetAgcHeqClipLimitLow(&_port, &currentClipLimitLow);

	if(result != LEP_OK) {
		std::cerr << "Error getting AGC Clip Limit Low. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AGC Clip Limit Low: " << currentClipLimitLow << std::endl;
	}
}

// Bin Extension 
void SetBinExtension(uint16_t binExtension) {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_SetAgcHeqBinExtension(&_port, binExtension);
	if(result != LEP_OK) {
		std::cerr << "Error setting AGC Bin Extension. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC Bin Extension set successfully" << std::endl;
	}
}

void GetBinExtension() {
	if(!_connected) {
		lepton_connect();
	}
	uint16_t currentBinExtension;
	LEP_RESULT result = LEP_GetAgcHeqBinExtension(&_port, &currentBinExtension);

	if(result != LEP_OK) {
		std::cerr << "Error getting AGC Bin Extension. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AGC Bin Extension: " << currentBinExtension << std::endl;
	}
}

// AGC Midpoint
void lepton_set_heq_midpoint(uint16_t midpoint) {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_SetAgcHeqMidPoint(&_port, midpoint);
	if(result != LEP_OK) {
		std::cerr << "Error setting AGC Midpoint. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC Midpoint set successfully" << std::endl;
	}
}
void lepton_get_agc_heq_midpoint() {
	if(!_connected) {
		lepton_connect();
	}
	uint16_t currentMidpoint;
	LEP_RESULT result = LEP_GetAgcHeqMidPoint(&_port, &currentMidpoint);

	if(result != LEP_OK) {
		std::cerr << "Error getting AGC Midpoint. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AGC Midpoint: " << currentMidpoint << std::endl;
	}
}

// Empty Counts
void SetEmptyCounts(uint16_t emptyCounts) {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_SetAgcHeqEmptyCount(&_port, emptyCounts);
	if(result != LEP_OK) {
		std::cerr << "Error setting AGC Empty Counts. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC Empty Counts set successfully" << std::endl;
	}
}
void GetEmptyCounts() {
	if(!_connected) {
		lepton_connect();
	}
	uint16_t currentEmptyCounts;
	LEP_RESULT result = LEP_GetAgcHeqEmptyCount(&_port, &currentEmptyCounts);

	if(result != LEP_OK) {
		std::cerr << "Error getting AGC Empty Counts. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AGC Empty Counts: " << currentEmptyCounts << std::endl;
	}
}


// Normalization Factor
void SetNormalizationFactor(uint16_t normalizationFactor) {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_SetAgcHeqNormalizationFactor(&_port, normalizationFactor);
	if(result != LEP_OK) {
		std::cerr << "Error setting AGC Normalization Factor. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC Normalization Factor set successfully" << std::endl;
	}
}
void GetNormalizationFactor() {
	if(!_connected) {
		lepton_connect();
	}
	uint16_t currentNormalizationFactor;
	LEP_RESULT result = LEP_GetAgcHeqNormalizationFactor(&_port, &currentNormalizationFactor);

	if(result != LEP_OK) {
		std::cerr << "Error getting AGC Normalization Factor. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AGC Normalization Factor: " << currentNormalizationFactor << std::endl;
	}
}

// HEQ 8-bit Scale Factor
void lepton_set_heq_8bit_scale_factor() {
    if(!_connected) {
        lepton_connect();
    }
    LEP_RESULT result = LEP_SetAgcHeqScaleFactor(&_port, LEP_AGC_SCALE_TO_8_BITS);
	if(result != LEP_OK) {
        std::cerr << "Error setting 8-bit HEQ Error code: " << result << std::endl;
    } else {
        std::cout << "8-bit HEQ set successfully" << std::endl;
    }
}
// HEQ 14-bit Scale Factor
void lepton_set_heq_14bit_scale_factor() {
    if(!_connected) {
        lepton_connect();
    }
    LEP_SetAgcHeqScaleFactor(&_port, LEP_AGC_SCALE_TO_14_BITS);
}
//Get Scale Factor of HEQ
void lepton_get_heq_scale_factor() {
    if(!_connected) {
        lepton_connect();
    }
    LEP_AGC_HEQ_SCALE_FACTOR_E currentScaleFactor;
    LEP_RESULT result = LEP_GetAgcHeqScaleFactor(&_port, &currentScaleFactor);

    if(result != LEP_OK) {
        std::cerr << "Error getting HEQ Scale Factor. Error code: " << result << std::endl;
    } else {
        std::cout << "Current HEQ Scale Factor: ";
        switch(currentScaleFactor) {
            case LEP_AGC_SCALE_TO_8_BITS:
                std::cout << "8-bit" << std::endl;
                break;
            case LEP_AGC_SCALE_TO_14_BITS:
                std::cout << "14-bit" << std::endl;
                break;
            default:
                std::cout << "Unknown" << std::endl;
                break;
        }
    }
}

// AGC Calculation State
void SetAgcCalculationState() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RESULT result = LEP_SetAgcCalcEnableState(&_port, LEP_AGC_ENABLE);
	if(result != LEP_OK) {
		std::cerr << "Error setting AGC Calculation State. Error code: " << result << std::endl;
	} else {
		std::cout << "AGC Calculation State set successfully" << std::endl;
	}
}

void GetAgcCalculationState() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_AGC_ENABLE_E currentAgcCalculationEnableState;
	LEP_RESULT result = LEP_GetAgcCalcEnableState(&_port, &currentAgcCalculationEnableState);

	if(result != LEP_OK) {
		std::cerr << "Error getting AGC Calculation State. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AGC Calculation State: " << currentAgcCalculationEnableState << std::endl;
	}
}

// Histogram Statistic
void lepton_get_histogram_statistics() {
    if (!_connected) {
        lepton_connect();
    }

    LEP_AGC_HISTOGRAM_STATISTICS_T currentHistogram;
    LEP_AGC_HISTOGRAM_STATISTICS_T_PTR currentHistogramPtr = &currentHistogram;
    
    LEP_RESULT result = LEP_GetAgcHistogramStatistics(&_port, &currentHistogramPtr);
    if (result != LEP_OK) {
        std::cerr << "Error getting Histogram Statistic. Error code: " << result << std::endl;
    } else {
        std::cout << "Current Histogram Statistic: " << std::endl;
        std::cout << "Histogram Min: " << currentHistogram.minIntensity << std::endl;
        std::cout << "Histogram Max: " << currentHistogram.maxIntensity << std::endl;
        std::cout << "Histogram Mean: " << currentHistogram.meanIntensity << std::endl;
        std::cout << "Histogram Population: " << currentHistogram.numPixels << std::endl;
    }
}


void lepton_reboot() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RunOemReboot(&_port);
}
