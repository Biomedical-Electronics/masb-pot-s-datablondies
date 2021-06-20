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
static double VObjetivo = 0;
static double samplingPeriod = 0;
static uint32_t point = 0;
static uint32_t difVertex = 0;
static double Vertexup = 0;
static double Vertexlow = 0;
static double Vertexmid = 0;
static double Vertexmid2 = 0;

//Conversions:
static double u2b_m = 8.0/3.3;
static double u2b_b = 4.0;

void CV_setUart(UART_HandleTypeDef *newHuart) {

	/*
	 * Function: CV_setUart
	 * ----------------------------
	 *   Sets the UART pheripherial for the voltammetry
	 *
	 *   *newHuart: memory pointer of the UART
	 *
	 *   returns: Nothing
	 */
	huart = newHuart;
}

void CV_setTimer(TIM_HandleTypeDef *newTimer){

	/*
	 * Function: CV_setTimer
	 * ----------------------------
	 *   Sets the timer pheripherial for the voltammetry
	 *
	 *   *newTimer: memory pointer of the timer
	 *
	 *   returns: Nothing
	 */

	timer = newTimer;
}

void CV_setADC(ADC_HandleTypeDef *newADC){

	/*
	 * Function: CV_setADC
	 * ----------------------------
	 *   Sets the ADC pheripherial for the voltammetry
	 *
	 *   *newADC: memory pointer of the ADC
	 *
	 *   returns: Nothing
	 */

	hadc = newADC;
}



void CV_settingConfiguration(void){

	/*
	 * Function: CV_settingConfiguration
	 * ----------------------------
	 *   Returns the cvConfiguration structure with the eDC, samplingPeriodMS
	 *   measurementTime values
	 *
	 *   returns: Nothing
	 */

	cvConfiguration = MASB_COMM_S_getCvConfiguration();
}

void CV_firstMeasure(MCP4725_Handle_T hdac){

	/*
	 * Function: CV_firstMeasure
	 * ----------------------------
	 *  Sets the timer's configuration, opens the relay and fixes the
	 *  cell's voltage with the DAC. Also it reads the first values
	 *  of the voltammetry measurement
	 *
	 *   MCP4725_Handle_T hdac: DAC pheripherial
	 *
	 *   returns: Nothing
	 */

	samplingPeriod = (cvConfiguration.eStep/cvConfiguration.scanRate)*1000;
	TIM_clearEstadoTest();
	__HAL_TIM_SET_AUTORELOAD(timer, samplingPeriod);
	__HAL_TIM_SET_COUNTER(timer, 0);
	HAL_TIM_Base_Start_IT(timer);

	float outputVoltage = cvConfiguration.eBegin / 2.0 + 2.0;
	MCP4725_SetOutputVoltage(hdac, outputVoltage);

	VObjetivo = cvConfiguration.eVertex1;

	Vcell = cvConfiguration.eBegin;

	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);


	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, 200);
	VADC = HAL_ADC_GetValue(hadc);
	double adcVoltage = ((double) VADC) / 4095.0 * 3.3;
	Vcell_real = -((adcVoltage*u2b_m)-u2b_b);

	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, 200);
	Vtia = HAL_ADC_GetValue(hadc);
	adcVoltage = ((double) Vtia) / 4095.0 * 3.3;
	Icell = ((adcVoltage*u2b_m)-u2b_b)/50e3f; //((adcVoltage - 1.65)*2)/12e3;

	point = 0;

	//To read the voltammetry measurements if Vertex1 > Vertex2 and viceversa.

	if (cvConfiguration.eVertex1 > cvConfiguration.eVertex2){
		difVertex = cvConfiguration.eVertex1 - cvConfiguration.eVertex2;
		Vertexlow = cvConfiguration.eVertex2;
		Vertexup = cvConfiguration.eVertex1;
		Vertexmid = cvConfiguration.eBegin;
		Vertexmid2 = cvConfiguration.eVertex2;
	} else {
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

	//To test it
	/*
	 * data.voltage = Vcell;
	 * data.current = Vcell / 10e3;
	 */

	MASB_COMM_S_sendData(data);

}

void CV_testing(MCP4725_Handle_T hdac){

	/*
	 * Function: CV_testing
	 * ----------------------------
	 *  This is the ISR of the timer. At a certain frequency
	 *  this function reads the voltammetry new V_CELL and I_CELL values .
	 *
	 *   MCP4725_Handle_T hdac: DAC pheripherial
	 *
	 *   returns: Nothing
	 */

		if (TIM_isPeriodElapsed()){

			TIM_clearEstadoTest();

			if (estadoEnviar){

				estadoEnviar = FALSE;
				point = point + 1;

				HAL_ADC_Start(hadc);
				HAL_ADC_PollForConversion(hadc, 200);
				VADC = HAL_ADC_GetValue(hadc);
				double adcVoltage = ((double) VADC) / 4095.0 * 3.3;
				Vcell_real =  -((adcVoltage*u2b_m)-u2b_b);

				HAL_ADC_Start(hadc);
				HAL_ADC_PollForConversion(hadc, 200);
				Vtia = HAL_ADC_GetValue(hadc);
				adcVoltage = ((double) Vtia) / 4095.0 * 3.3;
				Icell = ((adcVoltage*u2b_m)-u2b_b)/50e3f;

				data.point = point;
				data.timeMs = samplingPeriod*point;
				data.voltage = Vcell_real;
				data.current = Icell;

				//To test it:
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
						VObjetivo = Vertexmid;
					}
					else{
						Vcell = Vcell - cvConfiguration.eStep;
					}
				}
				else if (VObjetivo == Vertexup) {

					if ((Vcell + cvConfiguration.eStep) >= VObjetivo){
						Vcell = VObjetivo - cvConfiguration.eStep;
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
							VObjetivo = cvConfiguration.eVertex1;
						}
						else{
							Vcell = Vcell + cvConfiguration.eStep;

						}
					} else {
						if ((Vcell - cvConfiguration.eStep) <= VObjetivo){
							Vcell = VObjetivo + cvConfiguration.eStep;
							VDAC = 1.65 - (VObjetivo/2);
							VObjetivo = cvConfiguration.eVertex2;
						}
						else{
							Vcell = Vcell - cvConfiguration.eStep;

						}
					}
				}
		}

		//---------------------------------------------------------------------------------------------
		float outputVoltage =  Vcell / 2.0 + 2.0;
		MCP4725_SetOutputVoltage(hdac, outputVoltage);
	}
}

_Bool is_estadoCycle(void) {

	/*
	 * Function: is_estadoCycle
	 * ----------------------------
	 *   It returns the boolean of the
	 *   value estadoCycle
	 *
	 *   returns: estadoCycle.
	 */
	return estadoCycle;

}

void true_estadoCycle(MCP4725_Handle_T hdac) {

	/*
	 * Function: true_estadoCycle
	 * ----------------------------
	 *   Stop Command. If the HOST Stop button is pushed,
	 *   this function stops the measurement reading.
	 *
	 *   MCP4725_Handle_T hdac: DAC pheripherial
	 *
	 *   returns: the square of the larger of n1 and n2
	 */

	HAL_TIM_Base_Stop_IT(timer);
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
	estadoCycle = TRUE;

}
