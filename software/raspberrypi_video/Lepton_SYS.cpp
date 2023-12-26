#include "Lepton_SYS.h"
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

/*************************************************************************************************************/
/** SYS and OEM COMMANDS                                                                                            **/
/*************************************************************************************************************/

// AUX temp In kelvin
void lepton_get_aux_temp_kelvin() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_SYS_AUX_TEMPERATURE_KELVIN_T currentAuxTemp;
	LEP_RESULT result = LEP_GetSysAuxTemperatureKelvin(&_port, &currentAuxTemp);

	if(result != LEP_OK) {
		std::cerr << "Error getting AUX temp in kelvin. Error code: " << result << std::endl;
	} else {
		std::cout << "Current AUX temp in kelvin: " << currentAuxTemp << std::endl;
	}
}