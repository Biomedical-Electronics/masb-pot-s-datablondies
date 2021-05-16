#include "components/cyclic_voltammetry.h"
#include "main.h"
#include "components/masb_comm_s.h"

static ADC_HandleTypeDef  *hadc;
static TIM_HandleTypeDef *timer;
static _Bool estadoTest = FALSE;
struct CV_Configuration_S cvConfiguration;
struct Data_S data;

void CV_changeTimerConfiguration(TIM_HandleTypeDef *timer, double scanRate, double eStep){

	double samplingPeriodMs = 0.0;
	samplingPeriodMs = eStep/scanRate;

	__HAL_TIM_SET_AUTORELOAD(timer, samplingPeriodMs * 10); // Fijamos el periodo.

	__HAL_TIM_SET_COUNTER(timer, 0); // Reiniciamos el contador del timer a 0.
	HAL_TIM_Base_Start_IT(timer); // Iniciamos el timer.

}

void MASB_COMM_S_CV_auxiliar(
		double Vcellr,
		struct CV_Configuration_S config,
		struct Data_S data,
		uint32_t point
		){
	uint32_t VADC = 0;
	double Vcell = 0;
	uint32_t Vtia = 0;
	double Icell = 0;
	Vcellr = Vcellr + config.eStep; //supongo que eStep es negativo

    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 200);
    VADC = HAL_ADC_GetValue(hadc);
    Vcell = (1.65 - VADC)*2; //hay que ponerle incremento?

    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 200);
    Vtia = HAL_ADC_GetValue(hadc);
    Icell = ((Vtia - 1.65)*2)/12e3;

    data.point = point;
    data.timeMs = (config.eStep/config.scanRate)*(point);
    data.voltage = Vcell;
    data.current = Icell;
    MASB_COMM_S_sendData(data);
    point = point + 1;
}

void MASB_COMM_S_CV_testing(MCP4725_Handle_T hdac){
		//It only executes once:
		//uint32_t VADC = 0;
		//double Vcell = 0;
		double Vcellr = 0;
		//uint32_t Vtia = 0;
		//double Icell = 0;
		uint32_t counter = 0;
		uint32_t point = 1;
		uint32_t Vobj = 0; // Vobjetivo

		cvConfiguration = MASB_COMM_S_getCvConfiguration();
		CV_changeTimerConfiguration(timer, cvConfiguration.scanRate, cvConfiguration.eStep);
		float VDAC = 1.65 - (cvConfiguration.eBegin/2);
		MCP4725_SetOutputVoltage(hdac, VDAC); //fijamos tension vcell a ebegin

		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);

		while (counter < cvConfiguration.cycles) { //bucle de ciclo
			if (estadoTest){
				estadoTest = !estadoTest;
				Vobj = cvConfiguration.eVertex1; //primer vobj vertex1
				Vcellr = cvConfiguration.eBegin; //vcell real
				while (Vobj > Vcellr ){
					MASB_COMM_S_CV_auxiliar(Vcellr, cvConfiguration, data, point);
				}
				//alcanzado Vertex1
				Vcellr = Vobj;
				Vobj = cvConfiguration.eBegin; //segundo vobj eBegin
				while (Vcellr > Vobj){
					MASB_COMM_S_CV_auxiliar(Vcellr, cvConfiguration, data, point);
				}
				//alcanzado eBegin
				Vcellr = Vobj;
				Vobj = cvConfiguration.eVertex2; //tercer vobj Vertex2
				while (Vcellr > Vobj){
					MASB_COMM_S_CV_auxiliar(Vcellr, cvConfiguration, data, point);
				}
				//alcanzado Vertex2
				Vcellr = Vobj;
				Vobj = cvConfiguration.eBegin; //objetivo para completar ciclo
				while (Vcellr < Vobj){
					MASB_COMM_S_CV_auxiliar(Vcellr, cvConfiguration, data, point);
				}
			    counter = counter + 1; //incremento de 1 ciclo
			    point = 0; //al final de cada ciclo
			}
		}
		//HAL_TIM_Base_DeInit(timer);
		HAL_TIM_Base_Stop_IT(timer); //detener timer
		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}
