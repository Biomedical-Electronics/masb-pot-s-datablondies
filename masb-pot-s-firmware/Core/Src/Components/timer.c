/*
 * timer.c
 *
 *  Created on: 25 May 2021
 *      Author: Celia
 */

#include "components/timer.h"

#define TRUE 1
#define FALSE 0

volatile static _Bool estadoTest = FALSE;


void TIM_clearEstadoTest(void) {

	estadoTest = FALSE;

}

_Bool TIM_isPeriodElapsed(void) {

	return estadoTest;

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *timer) {

	estadoTest = TRUE;

}
