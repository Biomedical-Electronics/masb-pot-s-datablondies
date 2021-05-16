#ifndef INC_COMPONENTS_MASB_COMM_S_H_
#define INC_COMPONENTS_MASB_COMM_S_H_

#define UART_BUFF_SIZE		 50
#define UART_TERM_CHAR		 0x00
#define TRUE					1
#define FALSE					0

// Commands
#define START_CV_MEAS	0x01
#define START_CA_MEAS	0x02
#define STOP_MEAS		0x03

#include "stm32f4xx_hal.h"
#include "components/cobs.h"
#include "components/cyclic_voltammetry.h"
#include "components/chrono_amperometry.h"
#include "components/mcp4725_driver.h"

// Structures
struct Data_S {
	uint32_t point;
	uint32_t timeMs;
	double voltage;
	double current;
};

// Prototypes.
void MASB_COMM_S_setUart(UART_HandleTypeDef *newHuart);
void MASB_COMM_S_setTimer(TIM_HandleTypeDef *newTimer);
void MASB_COMM_S_setADC(ADC_HandleTypeDef *newADC);

void MASB_COMM_S_waitForMessage(void);
_Bool MASB_COMM_S_dataReceived(void);
uint8_t MASB_COMM_S_command(void);

struct CV_Configuration_S MASB_COMM_S_getCvConfiguration(void);
struct CA_Configuration_S MASB_COMM_S_getCaConfiguration(void);
void MASB_COMM_S_sendData(struct Data_S data);

void MASB_COMM_S_CA_testing(MCP4725_Handle_T hdac);

#endif /* INC_COMPONENTS_MASB_COMM_S_H_ */
