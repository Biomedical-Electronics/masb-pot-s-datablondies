#ifndef INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_
#define INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_

#include "stm32f4xx_hal.h"
#include "components/cobs.h"
#include "components/mcp4725_driver.h"
#include "components/masb_comm_s.h"
#include "main.h"
#include "components/stm32main.h"

struct CV_Configuration_S {
	double eBegin;
	double eVertex1;
	double eVertex2;
	uint8_t cycles;
	double scanRate;
	double eStep;
};

void CA_changeTimerConfiguration(TIM_HandleTypeDef *timer, long samplingPeriodMs, uint32_t freq_timer);
void MASB_COMM_S_CV_testing(MCP4725_Handle_T hdac);
void MASB_COMM_S_CV_auxiliar(
		double Vcellr,
		double eStep,
		double scanRate,
		struct Data_S data,
		uint32_t point
		);

#endif /* INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_ */
