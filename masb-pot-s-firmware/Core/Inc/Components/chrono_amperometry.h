#ifndef INC_COMPONENTS_CHRONO_AMPEROMETRY_H_
#define INC_COMPONENTS_CHRONO_AMPEROMETRY_H_


#include "stm32f4xx_hal.h"

struct CA_Configuration_S {
	double eDC;
	long samplingPeriodMs;
	long measurementTime;
};


#endif /* INC_COMPONENTS_CHRONO_AMPEROMETRY_H_ */
