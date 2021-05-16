#ifndef INC_COMPONENTS_CHRONO_AMPEROMETRY_H_
#define INC_COMPONENTS_CHRONO_AMPEROMETRY_H_


#include "stm32f4xx_hal.h"

struct CA_Configuration_S {

	double eDC;
	uint32_t samplingPeriodMs;
	uint32_t measurementTime;

};

void CA_changeTimerConfiguration(TIM_HandleTypeDef *timer, long samplingPeriodMs);

#endif /* INC_COMPONENTS_CHRONO_AMPEROMETRY_H_ */
