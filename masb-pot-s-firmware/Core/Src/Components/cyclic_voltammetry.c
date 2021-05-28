#include "components/cyclic_voltammetry.h"
#include "components/timer.h"

static UART_HandleTypeDef *huart;
static ADC_HandleTypeDef *hadc;
static TIM_HandleTypeDef *timer;

static _Bool estadoEnviar = FALSE;
static _Bool estadoCycle = TRUE;
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
static uint32_t counter = 0;
static double VObjetivo = 0;
static double samplingPeriod = 0;
static uint32_t point = 0;


void CV_setUart(UART_HandleTypeDef *newHuart) {
	huart = newHuart;
}

void CV_setTimer(TIM_HandleTypeDef *newTimer){
	timer = newTimer;
}

void CV_setADC(ADC_HandleTypeDef *newADC){
	hadc = newADC;
}



void CV_firstMeasure(MCP4725_Handle_T hdac){

	cvConfiguration = MASB_COMM_S_getCvConfiguration();

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

	data.point = point;
	data.timeMs = samplingPeriod*point;
	//					data.voltage = Vcell_real;
						data.voltage = Vcell;
	//					data.current = Icell;
						data.current = Vcell / 10e3;
	MASB_COMM_S_sendData(data);

}

void CV_testing(MCP4725_Handle_T hdac){

		while ((counter < cvConfiguration.cycles) & (estadoCycle)){

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
//					data.voltage = Vcell_real;
					data.voltage = Vcell;
//					data.current = Icell;
					data.current = Vcell / 10e3;
					MASB_COMM_S_sendData(data);

				}

				//-----------------------------------------------------------------------------------

				else if (Vcell == VObjetivo){

					if (VObjetivo == cvConfiguration.eVertex1){

						VObjetivo = cvConfiguration.eVertex2;
						Vcell = Vcell - cvConfiguration.eStep;
					}

					else if (VObjetivo == cvConfiguration.eVertex2){

						VObjetivo = cvConfiguration.eBegin;
						Vcell = Vcell + cvConfiguration.eStep;

					}
					else {
						if (counter == cvConfiguration.cycles){
							estadoCycle = FALSE;
						}
						counter = counter + 1;
						VObjetivo = cvConfiguration.eVertex1;
						Vcell = Vcell + cvConfiguration.eStep;
					}
				}

				//-------------------------------------------------------------------------------------

				else {

					estadoEnviar = TRUE;

					if (VObjetivo == cvConfiguration.eVertex2){

						if ((Vcell - cvConfiguration.eStep) <= VObjetivo){
							Vcell = VObjetivo;
							VDAC = 1.65 - (VObjetivo/2);
							MCP4725_SetOutputVoltage(hdac, VDAC);
						}
						else{
							Vcell = Vcell - cvConfiguration.eStep;
						}
					}

					else {

						if ((Vcell + cvConfiguration.eStep) >= VObjetivo){
							Vcell = VObjetivo;
							VDAC = 1.65 - (VObjetivo/2);
							MCP4725_SetOutputVoltage(hdac, VDAC);
						}
						else{
							Vcell = Vcell + cvConfiguration.eStep;
						}
					}
			}

			//---------------------------------------------------------------------------------------------
		}
	}
	HAL_TIM_Base_Stop_IT(timer);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}
