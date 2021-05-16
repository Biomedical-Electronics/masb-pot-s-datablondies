#include "components/chrono_amperometry.h"



void CA_changeTimerConfiguration(TIM_HandleTypeDef *timer, long samplingPeriodMs, uint32_t freq_timer){

	uint32_t counterPeriod = 0;
	uint32_t counterPeriodLast = 0;
	uint32_t prescalar = 0;

	//Using the timer formulas from p3:
	//counterPeriod = samplingPeriodMs * (84e6/samplingPeriodMs);

	if (counterPeriod > 65535){
		counterPeriodLast = samplingPeriodMs*(freq_timer/samplingPeriodMs);
		prescalar = counterPeriod/counterPeriodLast;
		__HAL_TIM_SET_PRESCALER(timer, prescalar);
		__HAL_TIM_SET_COUNTER(timer,counterPeriodLast);
	}

	__HAL_TIM_SET_COUNTER(timer, counterPeriod);


}
