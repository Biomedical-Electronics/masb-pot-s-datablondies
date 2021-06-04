#include "components/chrono_amperometry.h"
#include "components/timer.h"

static UART_HandleTypeDef *huart;
static ADC_HandleTypeDef *hadc;
static TIM_HandleTypeDef *timer;

static struct CA_Configuration_S caConfiguration;
volatile static uint32_t measurementTimeMs = 0;
static uint32_t VADC = 0;
static double Vcell = 0;
static uint32_t Vtia = 0;
static double Icell = 0;
volatile static uint32_t counter = 0;
static uint32_t point = 1;

//Conversions:
static double u2b_m = 8.0/3.3;
static double u2b_b = 4.0;

void CA_setUart(UART_HandleTypeDef *newHuart) {
	huart = newHuart;
}

void CA_setTimer(TIM_HandleTypeDef *newTimer){
	timer = newTimer;
}

void CA_setADC(ADC_HandleTypeDef *newADC){
	hadc = newADC;
}

void CA_settingConfiguration(void){
	caConfiguration = MASB_COMM_S_getCaConfiguration();
}

void CA_firstSample(MCP4725_Handle_T hdac){

	//caConfiguration = MASB_COMM_S_getCaConfiguration();
	TIM_clearEstadoTest();
	__HAL_TIM_SET_AUTORELOAD(timer, caConfiguration.samplingPeriodMs * 10);
	__HAL_TIM_SET_COUNTER(timer, 0);

	float outputVoltage = caConfiguration.eDC / 2.0 + 2.0;
	MCP4725_SetOutputVoltage(hdac, outputVoltage);

	HAL_TIM_Base_Start_IT(timer);

	measurementTimeMs = caConfiguration.measurementTime*1000;

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);


	HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 200);
    VADC = HAL_ADC_GetValue(hadc);
    double adcVoltage = ((double) VADC) / 4095.0 * 3.3;
    Vcell = -((adcVoltage*u2b_m)-u2b_b);

    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 200);
    Vtia = HAL_ADC_GetValue(hadc);
    adcVoltage = ((double) Vtia) / 4095.0 * 3.3;
    Icell = ((adcVoltage*u2b_m)-u2b_b)/50e3f;

	point =  1;
	counter = 0;

	struct Data_S data;

    data.point = point;
    data.timeMs = counter;
    data.voltage = Vcell;
    data.current = Icell;

    MASB_COMM_S_sendData(data);

    counter = counter + caConfiguration.samplingPeriodMs;

    point = point + 1;

    TIM_clearEstadoTest();
}

void CA_testing(MCP4725_Handle_T hdac){

		if (TIM_isPeriodElapsed()){
			TIM_clearEstadoTest();

			HAL_ADC_Start(hadc);
		    HAL_ADC_PollForConversion(hadc, 200);
		    VADC = HAL_ADC_GetValue(hadc);
		    double adcVoltage = ((double) VADC) / 4095.0 * 3.3;
		    Vcell = -((adcVoltage*u2b_m)-u2b_b);

		    HAL_ADC_Start(hadc);
		    HAL_ADC_PollForConversion(hadc, 200);
		    Vtia = HAL_ADC_GetValue(hadc);
		    adcVoltage = ((double) Vtia) / 4095.0 * 3.3;
		    Icell = ((adcVoltage*u2b_m)-u2b_b)/50e3f;

			struct Data_S data;
			data.point = point;
			data.timeMs = counter;

			data.voltage = Vcell;
			data.current = Icell;
			MASB_COMM_S_sendData(data);
			counter = counter + caConfiguration.samplingPeriodMs;
			point = point + 1;
		}

}

_Bool is_counter(void) { //nueva funcion

	return (counter <= measurementTimeMs);

}

void true_counter(MCP4725_Handle_T hdac) { //nueva funcion
	HAL_TIM_Base_Stop_IT(timer);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}
