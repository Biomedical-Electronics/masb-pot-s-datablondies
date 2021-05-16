#include <stdlib.h>
#include <stdint.h>

#ifndef __AD5280_H
#define __AD5280_H

typedef struct AD5280_Handle_S *AD5280_Handle_T;


AD5280_Handle_T AD5280_Init(void);

void AD5280_ConfigSlaveAddress(AD5280_Handle_T self,
        uint8_t slaveAddress);

void AD5280_ConfigWriteFunction(AD5280_Handle_T self,
        void (*write)(uint8_t slaveAddress, uint8_t *data, uint16_t length));

void AD5280_ConfigNominalResistorValue(AD5280_Handle_T self,
        float nominalResistorValue);

void AD5280_SetWBResistance(AD5280_Handle_T self,
        float wbResistorValue);

void AD5280_DeInit(AD5280_Handle_T self);

#endif
