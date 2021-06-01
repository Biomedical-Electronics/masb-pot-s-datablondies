#ifndef INC_COMPONENTS_STM32MAIN_H_
#define INC_COMPONENTS_STM32MAIN_H_

#include "stm32f4xx_hal.h"
#include "components/masb_comm_s.h" //Crono + Volta + HAL + COBS + DAC
#include "components/ad5280_driver.h"
#include "components/i2c_lib.h"

struct Handles_S {
	UART_HandleTypeDef *huart2; //UART_HandleTypeDef *huart;
	ADC_HandleTypeDef *hadc1;
	I2C_HandleTypeDef *hi2c1;
	TIM_HandleTypeDef *htim3;
};

//Prototypes
void setup(struct Handles_S *handles);
void loop(void);


#endif /* INC_COMPONENTS_STM32MAIN_H_ */
