#include "../../Inc/components/mcp4725_driver.h"


struct MCP4725_Handle_S {
    uint8_t slaveAddress;
    float voltageReference;
    void (*write)(uint8_t slaveAddress, uint8_t *data, uint16_t length);
};


MCP4725_Handle_T MCP4725_Init(void) {

    return malloc(sizeof(struct MCP4725_Handle_S));

}

void MCP4725_ConfigSlaveAddress(MCP4725_Handle_T self,
        uint8_t slaveAddress) {

    self->slaveAddress = slaveAddress;

}

void MCP4725_ConfigWriteFunction(MCP4725_Handle_T self,
        void (*write)(uint8_t slaveAddress, uint8_t *data, uint16_t length)) {

    self->write = write;

}

void MCP4725_ConfigVoltageReference(MCP4725_Handle_T self,
        float voltageReferenceValue) {

    self->voltageReference = voltageReferenceValue;

};

void MCP4725_SetOutputVoltage(MCP4725_Handle_T self,
        float outputVoltage) {


    // Al convertir de decimal a entero, simplemente se elimina la parte
    // decimal y se redondea al entero inmediatemente inferior (no el mas
    // cercano). Se le suma 0.5 al resultado para que se redondee al entero
    // mas cercano.
    uint16_t dacValue = (uint16_t)(outputVoltage * 4096.0f /
            self->voltageReference + 0.5f);

    uint8_t data[2] = { (uint8_t)(dacValue >> 8), (uint8_t)dacValue };

    self->write(self->slaveAddress, data, 2);

}

void MCP4725_DeInit(MCP4725_Handle_T self) {

    free(self);

}
