#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifndef __I2C_LIB_H
#define __I2C_LIB_H

// Comentar esta linea cuando se tenga la placa disponible.
#define I2C_DEBUG

/**
  * @brief  Inicaliza la libreria obteniendo un puntero al handle de I2C.
  * @param  i2cHandler Puntero a una estructura tipo I2C_HandleTypeDef
  * 		que contiene la configuracion del I2C especificado.
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
void I2C_Init(I2C_HandleTypeDef *i2cHandler);
void I2C_Write(uint8_t slaveAddress, uint8_t *data, uint16_t length);

#endif
