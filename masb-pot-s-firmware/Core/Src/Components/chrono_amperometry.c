#include "components/chrono_amperometry.h"

static UART_HandleTypeDef *huart;
static ADC_HandleTypeDef *hadc;
static TIM_HandleTypeDef *timer;

static _Bool estadoTest = FALSE;
static struct Data_S data;
static struct CA_Configuration_S caConfiguration;
static uint32_t measurementTimeMs = 0;
static uint32_t VADC = 0;
static double Vcell = 0;
static uint32_t Vtia = 0;
static double Icell = 0;
static uint32_t counter = 0;
static uint32_t point = 1;

void CA_setUart(UART_HandleTypeDef *newHuart) {
	huart = newHuart;
}

void CA_setTimer(TIM_HandleTypeDef *newTimer){
	timer = newTimer;
}

void CA_setADC(ADC_HandleTypeDef *newADC){
	hadc = newADC;
}


//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *timer) {
	//estadoTest = TRUE;
//}

void CA_firstSample(MCP4725_Handle_T hdac){

	caConfiguration = MASB_COMM_S_getCaConfiguration();

	__HAL_TIM_SET_AUTORELOAD(timer, caConfiguration.samplingPeriodMs * 10);
	__HAL_TIM_SET_COUNTER(timer, 0);

	float VDAC = 1.65 - (caConfiguration.eDC/2);

	MCP4725_SetOutputVoltage(hdac, VDAC);

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);

	HAL_TIM_Base_Start_IT(timer);

	measurementTimeMs = caConfiguration.measurementTime*1000;

	estadoTest = FALSE;

	HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 200);
    VADC = HAL_ADC_GetValue(hadc);
    Vcell = (1.65 - VADC)*2;
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 200);
    Vtia = HAL_ADC_GetValue(hadc);
    Icell = ((Vtia - 1.65)*2)/12e3;

    data.point = point;
    data.timeMs = counter;
    data.voltage = Vcell;
    data.current = Icell;

    MASB_COMM_S_sendData(data);

    counter = counter + caConfiguration.samplingPeriodMs;

    point = point + 1;

}

void CA_testing(MCP4725_Handle_T hdac){

		while (counter <= measurementTimeMs) {
			if (estadoTest){
				estadoTest = FALSE;
			    HAL_ADC_Start(hadc);
			    HAL_ADC_PollForConversion(hadc, 200);
			    VADC = HAL_ADC_GetValue(hadc);
			    Vcell = (1.65 - VADC)*2;
			    HAL_ADC_Start(hadc);
			    HAL_ADC_PollForConversion(hadc, 200);
			    Vtia = HAL_ADC_GetValue(hadc);
			    Icell = ((Vtia - 1.65)*2)/12e3;
			    data.point = point;
			    data.timeMs = counter;
			    data.voltage = Vcell;
			    data.current = Icell;
			    MASB_COMM_S_sendData(data);
			    counter = counter + caConfiguration.samplingPeriodMs;
			    point = point + 1;
			}

		}
		HAL_TIM_Base_Stop_IT(timer);
		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}
