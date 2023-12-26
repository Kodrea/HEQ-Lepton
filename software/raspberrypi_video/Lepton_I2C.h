#ifndef LEPTON_I2C
#define LEPTON_I2C
#include <stdint.h>
#include <string>    // added for get state functions

void lepton_perform_ffc();
void lepton_reboot();
int lepton_connect();


//Enable AGC
void lepton_enable_agc();          
void lepton_disable_agc();          
std::string check_agc_state();  
//Set AGC Policy
void lepton_set_policy_linear();
void lepton_set_policy_heq();    
void lepton_set_heq_8bit_scale_factor();
void lepton_set_heq_14bit_scale_factor();
void lepton_get_heq_scale_factor();
//Set AGC ROI
void lepton_set_agc_roi(uint16_t startCol, uint16_t startRow, uint16_t endCol, uint16_t endRow);
void lepton_get_agc_roi();
// HEQ Damping Factor
void SetHeqDampening(uint16_t dampingFactor);
void GetHeqDampening();
// HEQ Max Gain
void lepton_max_gain(uint16_t maxGain);
void lepton_get_agc_max_gain(); 
// HEQ Clip Limit High
void SetHeqClipHigh(uint16_t clipLimitHigh);
void GetHeqClipHigh();
// HEQ Clip Limit Low
void SetHeqClipLow(uint16_t clipLimitLow);
void GetHeqClipLow();
// Bin Extension
void SetBinExtension(uint16_t binExtension);
void GetBinExtension();
// Midpoint
void lepton_set_heq_midpoint(uint16_t midpoint);
void lepton_get_agc_heq_midpoint();   
// HEQ Empty Counts
void SetEmptyCounts(uint16_t emptyCounts);
void GetEmptyCounts();
// Normalization Factor
void SetNormalizationFactor(uint16_t normalizationFactor);
void GetNormalizationFactor();
// Calculation State
//void SetAgcCalculationState(uint16_t calculationState);
void GetAgcCalculationState();
// Get Histogram Statistics
void lepton_get_histogram_statistics();

/*********************************************************************************************************
 * SYS and OEM Commands
 ********************************************************************************************************/

void lepton_get_aux_temp_kelvin();  // AUX temp In kelvin
void lepton_get_scene_statistics(); // Scene Statistics
void lepton_set_scene_roi(uint16_t startCol, uint16_t startRow, uint16_t endCol, uint16_t endRow); // Scene ROI
void lepton_get_scene_roi(); // Scene ROI

#endif
