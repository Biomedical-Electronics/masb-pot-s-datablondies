#include "components/masb_comm_s.h"
#include "components/cobs.h"
#include "main.h"

static UART_HandleTypeDef *huart;
static ADC_HandleTypeDef  *hadc;
static TIM_HandleTypeDef *timer;

static _Bool dataReceived = FALSE;
static _Bool estadoTest = FALSE;

struct CV_Configuration_S cvConfiguration;
struct CA_Configuration_S caConfiguration;
struct Data_S data;


uint8_t rxBuffer[UART_BUFF_SIZE] = { 0 },
		txBuffer[UART_BUFF_SIZE] = { 0 };

uint8_t rxBufferDecoded[UART_BUFF_SIZE] = { 0 },
		txBufferDecoded[UART_BUFF_SIZE] = { 0 };

uint8_t rxIndex = 0;


// Prototypes.
static double saveByteArrayAsDoubleFromBuffer(uint8_t *buffer, uint8_t index);
static double saveByteArrayAsLongFromBuffer(uint8_t *buffer, uint8_t index);
static void saveLongAsByteArrayIntoBuffer(uint8_t *buffer, uint8_t index, uint32_t longVal);
static void saveDoubleAsByteArrayIntoBuffer(uint8_t *buffer, uint8_t index, double doubleVal);

// Prototypes
void MASB_COMM_S_setUart(UART_HandleTypeDef *newHuart) {
	huart = newHuart;
}

void MASB_COMM_S_setTimer(TIM_HandleTypeDef *newTimer){
	timer = newTimer;
}

void MASB_COMM_S_setADC(ADC_HandleTypeDef *newADC){
	hadc = newADC;
}

union Double_Converter {

	double d;
	uint8_t b[8];

} doubleConverter;

union Long_Converter {

	uint32_t l;
	uint8_t b[4];

} longConverter;

void MASB_COMM_S_waitForMessage(void) {

	dataReceived = FALSE;
	rxIndex = 0;
	HAL_UART_Receive_IT(huart, &rxBuffer[rxIndex], 1);

}

_Bool MASB_COMM_S_dataReceived(void) {

	if (dataReceived) {

		COBS_decode(rxBuffer, rxIndex, rxBufferDecoded);

	}

	return dataReceived;

}

uint8_t MASB_COMM_S_command(void) {

	return rxBufferDecoded[0];

}

struct CV_Configuration_S MASB_COMM_S_getCvConfiguration(void){

	struct CV_Configuration_S cvConfiguration;

	cvConfiguration.eBegin = saveByteArrayAsDoubleFromBuffer(rxBufferDecoded, 1);
	cvConfiguration.eVertex1 = saveByteArrayAsDoubleFromBuffer(rxBufferDecoded, 9);
	cvConfiguration.eVertex2 = saveByteArrayAsDoubleFromBuffer(rxBufferDecoded, 17);
	cvConfiguration.cycles = rxBufferDecoded[25];
	cvConfiguration.scanRate = saveByteArrayAsDoubleFromBuffer(rxBufferDecoded, 26);
	cvConfiguration.eStep = saveByteArrayAsDoubleFromBuffer(rxBufferDecoded, 34);

	return cvConfiguration;

}

struct CA_Configuration_S MASB_COMM_S_getCaConfiguration(void){

	struct CA_Configuration_S caConfiguration;

	caConfiguration.eDC = saveByteArrayAsDoubleFromBuffer(rxBufferDecoded, 1);
	caConfiguration.samplingPeriodMs = saveByteArrayAsLongFromBuffer(rxBufferDecoded, 9);
	caConfiguration.measurementTime = saveByteArrayAsLongFromBuffer(rxBufferDecoded, 13);

	return caConfiguration;

}

void MASB_COMM_S_sendData(struct Data_S data) {

	saveLongAsByteArrayIntoBuffer(txBufferDecoded, 0, data.point);
	saveLongAsByteArrayIntoBuffer(txBufferDecoded, 4, data.timeMs);
	saveDoubleAsByteArrayIntoBuffer(txBufferDecoded, 8, data.voltage);
	saveDoubleAsByteArrayIntoBuffer(txBufferDecoded, 16, data.current);

	uint32_t txBufferLenght = COBS_encode(txBufferDecoded, 24, txBuffer);

	txBuffer[txBufferLenght] = UART_TERM_CHAR;
	txBufferLenght++;

	//while(!(huart2.gState == HAL_UART_STATE_READY));
	HAL_UART_Transmit_IT(huart, txBuffer, txBufferLenght);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (rxBuffer[rxIndex] == UART_TERM_CHAR) {
		dataReceived = TRUE;
	} else {
		rxIndex++;
		HAL_UART_Receive_IT(huart, &rxBuffer[rxIndex], 1);
	}

}

static double saveByteArrayAsDoubleFromBuffer(uint8_t *buffer, uint8_t index) {

	for (uint8_t i = 0; i < 8 ; i++) {
		doubleConverter.b[i] = buffer[i + index];
	}

	return doubleConverter.d;
}

static double saveByteArrayAsLongFromBuffer(uint8_t *buffer, uint8_t index) {

	for (uint8_t i = 0; i < 4 ; i++) {
		longConverter.b[i] = buffer[i + index];
	}

	return longConverter.l;
}


static void saveLongAsByteArrayIntoBuffer(uint8_t *buffer, uint8_t index, uint32_t longVal) {

	longConverter.l = longVal;

	for (uint8_t i = 0; i < 4 ; i++) {
		buffer[i + index] = longConverter.b[i];
	}
}

static void saveDoubleAsByteArrayIntoBuffer(uint8_t *buffer, uint8_t index, double doubleVal) {

	doubleConverter.d = doubleVal;

	for (uint8_t i = 0; i < 8 ; i++) {
		buffer[i + index] = doubleConverter.b[i];
	}
}

void MASB_COMM_S_CA_testing(MCP4725_Handle_T hdac){
		//It only executes once:
		uint32_t VADC = 0;
		double Vcell = 0;
		uint32_t Vtia = 0;
		double Icell = 0;
		uint32_t freqTimer = 10e3; //f = 10Hz for the timer
		uint32_t counter = 0;
		uint32_t point = 1;

		caConfiguration = MASB_COMM_S_getCaConfiguration();
		CA_changeTimerConfiguration(timer, caConfiguration.samplingPeriodMs, freqTimer); //!!!!!!!!!! CAMBIAR A MASB_COMM
		float VDAC = 1.65 - (caConfiguration.eDC/2);
		MCP4725_SetOutputVoltage(hdac, VDAC);

		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
		//HAL_TIM_Base_Init(timer);

		while (counter < caConfiguration.measurementTime) {
			if (estadoTest){
				estadoTest = !estadoTest;
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
		//HAL_TIM_Base_DeInit(timer);
		HAL_TIM_Base_Stop_IT(timer); //detener timer
		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *timer) {

	estadoTest = !estadoTest;

}
