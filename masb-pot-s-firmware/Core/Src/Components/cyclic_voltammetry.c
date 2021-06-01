#include "components/cyclic_voltammetry.h"
#include "components/timer.h"

static UART_HandleTypeDef *huart;
static ADC_HandleTypeDef *hadc;
static TIM_HandleTypeDef *timer;

static _Bool estadoEnviar = FALSE;
volatile static _Bool estadoCycle = TRUE;
static struct Data_S data;
static struct CV_Configuration_S cvConfiguration;

//Measurement
static uint32_t VADC = 0;
static double Vcell_real = 0;
static uint32_t Vtia = 0;
static double Icell = 0;

//Testing
static float VDAC = 0;
static double Vcell = 0;
//static uint32_t counter = 0;
static double VObjetivo = 0;
static double samplingPeriod = 0;
static uint32_t point = 0;
static uint32_t difVertex = 0;
static double Vertexup = 0;
static double Vertexlow = 0;
static double Vertexmid = 0;
static double Vertexmid2 = 0;

void CV_setUart(UART_HandleTypeDef *newHuart) {
	huart = newHuart;
}

void CV_setTimer(TIM_HandleTypeDef *newTimer){
	timer = newTimer;
}

void CV_setADC(ADC_HandleTypeDef *newADC){
	hadc = newADC;
}



void CV_settingConfiguration(void){
	cvConfiguration = MASB_COMM_S_getCvConfiguration();
}

void CV_firstMeasure(MCP4725_Handle_T hdac){

	//cvConfiguration = MASB_COMM_S_getCvConfiguration();
	samplingPeriod = (cvConfiguration.eStep/cvConfiguration.scanRate)*1000;
	TIM_clearEstadoTest();
	__HAL_TIM_SET_AUTORELOAD(timer, samplingPeriod);
	__HAL_TIM_SET_COUNTER(timer, 0);
	HAL_TIM_Base_Start_IT(timer);

	VDAC = 1.65 - (cvConfiguration.eBegin/2);
	MCP4725_SetOutputVoltage(hdac, VDAC);

	VObjetivo = cvConfiguration.eVertex1;

	Vcell = cvConfiguration.eBegin;

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);


	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, 200);
	VADC = HAL_ADC_GetValue(hadc);
	Vcell_real = (1.65 - VADC)*2;

	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, 200);
	Vtia = HAL_ADC_GetValue(hadc);
	Icell = ((Vtia - 1.65)*2)/12e3;
	point = 0;
	if (cvConfiguration.eVertex1 > cvConfiguration.eVertex2){
		difVertex = cvConfiguration.eVertex1 - cvConfiguration.eVertex2;
		Vertexlow = cvConfiguration.eVertex2;
		Vertexup = cvConfiguration.eVertex1;
		Vertexmid = cvConfiguration.eBegin;
		Vertexmid2 = cvConfiguration.eVertex2;
	} else { //consideramos que vertex1 es menor que vertex2
		difVertex = cvConfiguration.eVertex2 - cvConfiguration.eVertex1;
		Vertexlow = cvConfiguration.eVertex1;
		Vertexup = cvConfiguration.eVertex2;
		Vertexmid = cvConfiguration.eVertex2;
		Vertexmid2 = cvConfiguration.eVertex1;
	}

	data.point = point;
	data.timeMs = samplingPeriod*point;
	data.voltage = Vcell_real;
	data.current = Icell;
	//descomentar per fer proba
	/*
	 * data.voltage = Vcell;
	 * data.current = Vcell / 10e3;
	 */

	MASB_COMM_S_sendData(data);

}

void CV_testing(MCP4725_Handle_T hdac){

	//quitamos while

		if (TIM_isPeriodElapsed()){

			TIM_clearEstadoTest();

			if (estadoEnviar){

				estadoEnviar = FALSE;
				point = point + 1;

				HAL_ADC_Start(hadc);
				HAL_ADC_PollForConversion(hadc, 200);
				VADC = HAL_ADC_GetValue(hadc);
				Vcell_real = (1.65 - VADC)*2;

				HAL_ADC_Start(hadc);
				HAL_ADC_PollForConversion(hadc, 200);
				Vtia = HAL_ADC_GetValue(hadc);
				Icell = ((Vtia - 1.65)*2)/12e3;

				data.point = point;
				data.timeMs = samplingPeriod*point;
				data.voltage = Vcell_real;
				data.current = Icell;
				//descomentar per fer proba
				/*
				 * data.voltage = Vcell;
				 * data.current = Vcell / 10e3;
				 */

				MASB_COMM_S_sendData(data);

				if (point == 1+(cvConfiguration.cycles*2*(difVertex))/cvConfiguration.eStep){
					estadoCycle = FALSE;
				}

			}

			else {

				estadoEnviar = TRUE;

				if (VObjetivo == Vertexlow){

					if ((Vcell - cvConfiguration.eStep) <= VObjetivo){
						Vcell = VObjetivo + cvConfiguration.eStep;
						VDAC = 1.65 - (VObjetivo/2);
						MCP4725_SetOutputVoltage(hdac, VDAC);
						VObjetivo = Vertexmid;
					}
					else{
						Vcell = Vcell - cvConfiguration.eStep;
					}
				}

				else if (VObjetivo == Vertexup) {

					if ((Vcell + cvConfiguration.eStep) >= VObjetivo){
						Vcell = VObjetivo - cvConfiguration.eStep;
						VDAC = 1.65 - (VObjetivo/2);
						MCP4725_SetOutputVoltage(hdac, VDAC);
						VObjetivo = Vertexmid2;
					}
					else{
						Vcell = Vcell + cvConfiguration.eStep;
					}
				}
				else {
					if (cvConfiguration.eVertex1 > cvConfiguration.eVertex2){
						if ((Vcell + cvConfiguration.eStep) >= VObjetivo){
							Vcell = VObjetivo - cvConfiguration.eStep;
							VDAC = 1.65 - (VObjetivo/2);
							MCP4725_SetOutputVoltage(hdac, VDAC);
							VObjetivo = cvConfiguration.eVertex1;
						}
						else{
							Vcell = Vcell + cvConfiguration.eStep;

						}
					} else {
						if ((Vcell - cvConfiguration.eStep) <= VObjetivo){
							Vcell = VObjetivo + cvConfiguration.eStep;
							VDAC = 1.65 - (VObjetivo/2);
							MCP4725_SetOutputVoltage(hdac, VDAC);
							VObjetivo = cvConfiguration.eVertex2;
						}
						else{
							Vcell = Vcell - cvConfiguration.eStep;

						}
					}

				}
		}

		//---------------------------------------------------------------------------------------------

	}
}
_Bool is_estadoCycle(void) { //nueva funcion

	return estadoCycle;

}

void true_estadoCycle(MCP4725_Handle_T hdac) { //nueva funcion
	HAL_TIM_Base_Stop_IT(timer);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
	estadoCycle = TRUE;

}
