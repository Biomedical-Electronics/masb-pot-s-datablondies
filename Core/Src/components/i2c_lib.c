#include "../../Inc/components/I2C_lib.h"

static I2C_HandleTypeDef *hi2c;

void I2C_Init(I2C_HandleTypeDef *i2cHandler) {

    hi2c = i2cHandler;
    
}

void I2C_Write(uint8_t slaveAddress, uint8_t *data, uint16_t length) {

#ifndef I2C_DEBUG
    HAL_I2C_Master_Transmit(hi2c, slaveAddress, data, length, 100);
#endif

} 
