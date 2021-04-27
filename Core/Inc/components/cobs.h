/**
  ******************************************************************************
  * @file		cobs.h
  * @brief		Archivo de encabezado de (de)codificación en COBS.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */
#ifndef INC_COMPONENTS_COBS_H_
#define INC_COMPONENTS_COBS_H_

#include "stm32f4xx_hal.h"

/* Funciones exportadas ------------------------------------------------------*/
/** @defgroup COBS_Exported_Functions Consistent Overhead Byte Stuffing
  * @brief	Módulo para la (de)codificación de paquetes de bytes en COBS.
  */
/** @addtogroup COBS_Exported_Functions
  * @{
  */

uint32_t COBS_encode(uint8_t *decodedMessage, uint32_t lenght, uint8_t *codedMessage);
uint32_t COBS_decode(uint8_t *codedMessage, uint32_t lenght, uint8_t *decodedMessage);

/**
  * @}
  */

#endif /* INC_COMPONENTS_COBS_H_ */
