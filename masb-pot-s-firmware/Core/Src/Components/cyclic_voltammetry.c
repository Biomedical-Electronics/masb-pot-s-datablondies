#include "components/cyclic_voltammetry.h"

static UART_HandleTypeDef *huart;
static ADC_HandleTypeDef *hadc;
static TIM_HandleTypeDef *timer;

static _Bool estadoTest = FALSE;
static struct Data_S data;
static struct CV_Configuration_S cvConfiguration;

//Measurement
static uint32_t VADC = 0;
static double Vcell_real = 0;
static uint32_t Vtia = 0;
static double Icell = 0;

//Testing
static double Vcell = 0;
static uint32_t counter = 0;
static uint32_t VObjetivo = 0;
static double samplingPeriod = 0;
static uint32_t point = 1;


void CV_setUart(UART_HandleTypeDef *newHuart) {
	huart = newHuart;
}

void CV_setTimer(TIM_HandleTypeDef *newTimer){
	timer = newTimer;
}

void CV_setADC(ADC_HandleTypeDef *newADC){
	hadc = newADC;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *timer) {
	estadoTest = TRUE;
}
void CV_testing(MCP4725_Handle_T hdac){

		cvConfiguration = MASB_COMM_S_getCvConfiguration();

		samplingPeriod = (cvConfiguration.eStep/cvConfiguration.scanRate)*1000;
		__HAL_TIM_SET_AUTORELOAD(timer, samplingPeriod);
		__HAL_TIM_SET_COUNTER(timer, 0);
		HAL_TIM_Base_Start_IT(timer);

		float VDAC = 1.65 - (cvConfiguration.eBegin/2);
		MCP4725_SetOutputVoltage(hdac, VDAC);

		VObjetivo = cvConfiguration.eVertex1;

		Vcell = cvConfiguration.eBegin;

		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);

		while (counter <= cvConfiguration.cycles){

			if (estadoTest){

				estadoTest = FALSE;
				HAL_ADC_Start(hadc);
				HAL_ADC_PollForConversion(hadc, 200);
				VADC = HAL_ADC_GetValue(hadc);
				Vcell_real = (1.65 - VADC)*2;

				HAL_ADC_Start(hadc);
				HAL_ADC_PollForConversion(hadc, 200);
				Vtia = HAL_ADC_GetValue(hadc);
				Icell = ((Vtia - 1.65)*2)/12e3;

				data.point = point;
				data.timeMs = (cvConfiguration.eStep/cvConfiguration.scanRate)*(point);
				data.voltage = Vcell;
				data.current = Icell;
				MASB_COMM_S_sendData(data);

				if (Vcell == VObjetivo){

					if (VObjetivo == cvConfiguration.eVertex1){

						VObjetivo = cvConfiguration.eVertex2;
					}

					else if (VObjetivo == cvConfiguration.eVertex2){

						VObjetivo = cvConfiguration.eBegin;
						point = 1;
					}
					else {
						if (counter == cvConfiguration.cycles){
							break;
						}
						VObjetivo = cvConfiguration.eVertex1;
						counter = counter + 1;
					}
				}

				else {

					if (VObjetivo == cvConfiguration.eVertex2){ //!!!!
						if ((Vcell - cvConfiguration.eStep) < VObjetivo){
							VDAC = 1.65 - (VObjetivo/2);
							MCP4725_SetOutputVoltage(hdac, VDAC);
						}
						else{
							Vcell = Vcell - cvConfiguration.eStep;
						}
					}

					else {

						if ((Vcell + cvConfiguration.eStep) > VObjetivo){
							VDAC = 1.65 - (VObjetivo/2);
							MCP4725_SetOutputVoltage(hdac, VDAC);
						}
						else{
							Vcell = Vcell + cvConfiguration.eStep;
						}
					}
				point = point + 1;
			}

		}
	}
	HAL_TIM_Base_Stop_IT(timer);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}
