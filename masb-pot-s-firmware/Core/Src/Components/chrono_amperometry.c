#include "components/chrono_amperometry.h"


<<<<<<< HEAD
=======

void CA_changeTimerConfiguration(TIM_HandleTypeDef *timer, long samplingPeriodMs, uint32_t freq_timer){
	//codigo github
	__HAL_TIM_SET_AUTORELOAD(timer, samplingPeriodMs * 10); // Fijamos el periodo.
	// El mutliplicar el samplingPeriodMs por 10 para fijar el periodo solo es valido
	// si se fija una frecuencia de trabajo para el timer de 10 kHz.
	__HAL_TIM_SET_COUNTER(timer, 0); // Reiniciamos el contador del timer a 0.
	HAL_TIM_Base_Start_IT(timer); // Iniciamos el timer.

}
>>>>>>> parent of 654badb... Merge branch 'feature/timer' into hotfix/timer
