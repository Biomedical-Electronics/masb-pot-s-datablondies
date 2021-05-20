#ifndef INC_COMPONENTS_CHRONO_AMPEROMETRY_H_
#define INC_COMPONENTS_CHRONO_AMPEROMETRY_H_

#define TRUE 	1
#define FALSE	0

#include "stm32f4xx_hal.h"
#include "components/masb_comm_s.h"
#include "components/mcp4725_driver.h"
#include "main.h"

struct CA_Configuration_S {
	double eDC;
	long samplingPeriodMs;
	long measurementTime;
};

//Prototypes:

void CA_setUart(UART_HandleTypeDef *newHuart);
void CA_setTimer(TIM_HandleTypeDef *newTimer);
void CA_setADC(ADC_HandleTypeDef *newADC);
void CA_testing(MCP4725_Handle_T hdac);
void CA_firstSample(MCP4725_Handle_T hdac);


#endif /* INC_COMPONENTS_CHRONO_AMPEROMETRY_H_ */
