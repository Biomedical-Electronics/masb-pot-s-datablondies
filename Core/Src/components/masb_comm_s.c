/**
  ******************************************************************************
  * @file		masb_comm_s.c
  * @brief		Gestión de la comunicación con el host. Versión simplificada de
  *             MASB-COMM.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */


#include "components/masb_comm_s.h"
#include "components/cobs.h"

extern UART_HandleTypeDef huart2;

uint8_t rxBuffer[UART_BUFF_SIZE] = { 0 },
		txBuffer[UART_BUFF_SIZE] = { 0 };

uint8_t rxBufferDecoded[UART_BUFF_SIZE] = { 0 },
		txBufferDecoded[UART_BUFF_SIZE] = { 0 };

uint8_t rxIndex = 0;

_Bool dataReceived = FALSE;

// Prototypes.
static double saveByteArrayAsDoubleFromBuffer(uint8_t *buffer, uint8_t index);
static double saveByteArrayAsLongFromBuffer(uint8_t *buffer, uint8_t index);
static void saveLongAsByteArrayIntoBuffer(uint8_t *buffer, uint8_t index, uint32_t longVal);
static void saveDoubleAsByteArrayIntoBuffer(uint8_t *buffer, uint8_t index, double doubleVal);

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
	HAL_UART_Receive_IT(&huart2, &rxBuffer[rxIndex], 1);

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

	while(!(huart2.gState == HAL_UART_STATE_READY));
	HAL_UART_Transmit_IT(&huart2, txBuffer, txBufferLenght);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (rxBuffer[rxIndex] == UART_TERM_CHAR) {
		dataReceived = TRUE;
	} else {
		rxIndex++;
		HAL_UART_Receive_IT(&huart2, &rxBuffer[rxIndex], 1);
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
