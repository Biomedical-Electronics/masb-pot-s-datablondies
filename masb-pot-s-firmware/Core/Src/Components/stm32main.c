#include "components/stm32main.h"

AD5280_Handle_T hpot = NULL;
MCP4725_Handle_T hdac = NULL;


void setup(struct Handles_S *handles) {

	MASB_COMM_S_setUart(handles->huart2); //!!!!!!!!!!!!!!!!
	MASB_COMM_S_setTimer(handles->htim3);
	MASB_COMM_S_setADC(handles->hadc1);
	I2C_Init(handles->hi2c1);

	hpot = AD5280_Init();
	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	AD5280_ConfigWriteFunction(hpot, I2C_Write); // MIRAR I2C!!
	AD5280_SetWBResistance(hpot, 10e3f); //10kohms!!

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
				break;

 			case START_CA_MEAS:
 				MASB_COMM_S_CA_testing(hdac);
 				break;

			case STOP_MEAS:
 				break;

 			default:
 				break;

 		}

 		MASB_COMM_S_waitForMessage();

 	}

 	// Aqui es donde deberia de ir el codigo de control de las mediciones si se quiere implementar
   // el comando de STOP.

}
