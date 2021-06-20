#include "components/stm32main.h"

AD5280_Handle_T hpot = NULL;
MCP4725_Handle_T hdac = NULL;
static _Bool estadoCA = FALSE;
static _Bool estadoCV = FALSE;
static _Bool estadoIDLE = FALSE;
static _Bool primeraCA = TRUE;
static _Bool primeraCV = TRUE;


void setup(struct Handles_S *handles) {

	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_SET);

	HAL_Delay(500);

	MASB_COMM_S_setUart(handles->huart2);
	MASB_COMM_S_setTimer(handles->htim3);
	MASB_COMM_S_setADC(handles->hadc1);

	CA_setUart(handles->huart2);
	CA_setTimer(handles->htim3);
	CA_setADC(handles->hadc1);

	CV_setUart(handles->huart2);
	CV_setTimer(handles->htim3);
	CV_setADC(handles->hadc1);

	I2C_Init(handles->hi2c1);

	hpot = AD5280_Init();
	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	AD5280_ConfigWriteFunction(hpot, I2C_Write);
	AD5280_SetWBResistance(hpot, 50e3f);

	hdac = MCP4725_Init();
	MCP4725_ConfigSlaveAddress(hdac, 0x66);
	MCP4725_ConfigVoltageReference(hdac, 4.0f);
	MCP4725_ConfigWriteFunction(hdac, I2C_Write);


	MASB_COMM_S_waitForMessage();
}

void loop(void) {

    if (MASB_COMM_S_dataReceived()) {

 		switch(MASB_COMM_S_command()) {

 			case START_CV_MEAS:
 				CV_settingConfiguration();
 				estadoCV = TRUE;
 				estadoIDLE = FALSE;
 				break;

 			case START_CA_MEAS:
 				CA_settingConfiguration();
 				estadoCA = TRUE;
 				estadoIDLE = FALSE;
 				break;

			case STOP_MEAS:
				estadoIDLE = TRUE;

 				break;

 			default:
 				break;
 		}

 		MASB_COMM_S_waitForMessage();
 	}

    else{
    	if (estadoIDLE){
    		estadoIDLE = FALSE;
    		true_estadoCycle(hdac);
    		true_counter(hdac);
    		primeraCV = TRUE;
    		estadoCV = FALSE;
			estadoCA = FALSE;
			primeraCA = TRUE;
    	} else {
			if (estadoCV){
				if(primeraCV){
					CV_firstMeasure(hdac);
					primeraCV = FALSE;
				} else {
					if (is_estadoCycle()){
						CV_testing(hdac);
					} else {
						true_estadoCycle(hdac);
						estadoCV = FALSE;
						primeraCV = TRUE;
					}
				}
			}
			else if (estadoCA){
				if(primeraCA){
					CA_firstSample(hdac);
					primeraCA = FALSE;
				}else{
					if (is_counter()){
						CA_testing(hdac);
					} else {
						true_counter(hdac);
						estadoCA = FALSE;
						primeraCA = TRUE;
					}
				}
			}
    	}
    }
}
