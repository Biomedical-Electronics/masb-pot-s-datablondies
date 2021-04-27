/**
  ******************************************************************************
  * @file		masb_comm_s.h
  * @brief		Archivo de encabezado del componente encargado de la 
  *             comunicación con el host. Versión simplificada del protocolo
  *             MASB-COMM.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#ifndef INC_COMPONENTS_MASB_COMM_S_H_
#define INC_COMPONENTS_MASB_COMM_S_H_

#include "stm32f4xx_hal.h"
#include "components/cyclic_voltammetry.h"
#include "components/chronoamperometry.h"

#define UART_BUFF_SIZE		50
#define UART_TERM_CHAR		0x00

#define START_CV_MEAS		0x01
#define START_CA_MEAS		0x02
#define STOP_MEAS			0x03

#ifndef TRUE
    #define TRUE				1
#endif

#ifndef FALSE
    #define FALSE				0
#endif

struct Data_S {

	uint32_t point;
	uint32_t timeMs;
	double voltage;
	double current;

};

// Prototypes.
void MASB_COMM_S_waitForMessage(void);
_Bool MASB_COMM_S_dataReceived(void);
uint8_t MASB_COMM_S_command(void);
struct CV_Configuration_S MASB_COMM_S_getCvConfiguration(void);
void MASB_COMM_S_sendData(struct Data_S data);
struct CA_Configuration_S MASB_COMM_S_getCaConfiguration(void);

#endif /* INC_COMPONENTS_MASB_COMM_S_H_ */
