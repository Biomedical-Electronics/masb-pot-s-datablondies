/*
 * timer.h
 *
 *  Created on: 25 May 2021
 *      Author: Celia
 */

#ifndef INC_COMPONENTS_TIMER_H_
#define INC_COMPONENTS_TIMER_H_

#include "stm32f4xx_hal.h"

void TIM_clearEstadoTest(void);
_Bool TIM_isPeriodElapsed(void);

#endif /* INC_COMPONENTS_TIMER_H_ */
