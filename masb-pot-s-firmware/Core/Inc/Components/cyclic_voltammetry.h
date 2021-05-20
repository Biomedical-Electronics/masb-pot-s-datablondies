#ifndef INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_
#define INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_

#define TRUE	1
#define FALSE	0

#include "stm32f4xx_hal.h"
#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"
#include "main.h"

struct CV_Configuration_S {
	double eBegin;
	double eVertex1;
	double eVertex2;
	uint8_t cycles;
	double scanRate;
	double eStep;
};

//Prototypes

void CV_setUart(UART_HandleTypeDef *newHuart);
void CV_setTimer(TIM_HandleTypeDef *newTimer);
void CV_setADC(ADC_HandleTypeDef *newADC);
void CV_testing(MCP4725_Handle_T hdac);


#endif /* INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_ */
