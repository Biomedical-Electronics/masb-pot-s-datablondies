#include "components/stm32main.h"

AD5280_Handle_T hpot = NULL;
MCP4725_Handle_T hdac = NULL;
static _Bool estadoCA = FALSE;
static _Bool estadoCV = FALSE;
static _Bool estadoIDLE = FALSE;
static _Bool primeraCA = TRUE;
static _Bool primeraCV = TRUE;


void setup(struct Handles_S *handles) {

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
    	//si no recibo instruccion
    	if (estadoIDLE){ //estado = IDLE por lo tanto STOP
    		estadoIDLE = FALSE;
    		true_estadoCycle(hdac); //STOP -> abrir rele + parar timer
    		true_counter(hdac);
    		primeraCV = TRUE; //nueva variable para llamar primera medicion
    		estadoCV = FALSE; //queremos que despues del STOP la siguiente medida se haga con START
			estadoCA = FALSE;
			primeraCA = TRUE;
    	} else { //estado != IDLE
			if (estadoCV){ //estado = CV
				if(primeraCV){ //primera medida CV
					CV_firstMeasure(hdac);
					primeraCV = FALSE; //variable primera medida FALSE
				} else {
					if (is_estadoCycle()){ //if para controlar cada medida/punto/sample
						CV_testing(hdac); //funcion para obtener y enviar punto
					} else { //hemos completado la medida
						true_estadoCycle(hdac); //abrir rele + parar timer
						estadoCV = FALSE;
						primeraCV = TRUE;
					}
				}
			}
			//lo mismo pero para la CA
			else if (estadoCA){
				if(primeraCA){
					CA_firstSample(hdac);
					primeraCA = FALSE;
				}else{
					if (is_counter()){ //if para seguir con la medicion
						CA_testing(hdac);
					} else { //en este entra si acabamos la medicion, no si la paramos
						true_counter(hdac); //abrir rele + parar timer
						estadoCA = FALSE;
						primeraCA = TRUE;
					}
				}
			}
    	}
    }
}
