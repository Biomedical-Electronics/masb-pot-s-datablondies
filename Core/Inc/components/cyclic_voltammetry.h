/**
  ******************************************************************************
  * @file		cyclic_voltammetry.h
  * @brief		Archivo de encabezado del componente encargado de la gestión de
  *             la voltammetría cíclica.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#ifndef INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_
#define INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_

#include "stm32f4xx_hal.h"

struct CV_Configuration_S {

	double eBegin;
	double eVertex1;
	double eVertex2;
	uint8_t cycles;
	double scanRate;
	double eStep;

};

#endif /* INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_ */
