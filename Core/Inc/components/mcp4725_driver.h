#include <stdlib.h>
#include <stdint.h>

#ifndef __MCP4725_H
#define __MCP4725_H

typedef struct MCP4725_Handle_S *MCP4725_Handle_T;


MCP4725_Handle_T MCP4725_Init(void);

void MCP4725_ConfigSlaveAddress(MCP4725_Handle_T self,
        uint8_t slaveAddress);

void MCP4725_ConfigWriteFunction(MCP4725_Handle_T self,
        void (*write)(uint8_t slaveAddress, uint8_t *data, uint16_t length));

void MCP4725_ConfigVoltageReference(MCP4725_Handle_T self,
        float voltageReferenceValue);

void MCP4725_SetOutputVoltage(MCP4725_Handle_T self,
        float outputVoltage);

void MCP4725_DeInit(MCP4725_Handle_T self);

#endif
