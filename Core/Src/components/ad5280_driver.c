#include "../../Inc/components/ad5280_driver.h"

struct AD5280_Handle_S {
    uint8_t slaveAddress;
    float nominalResistorValue;
    uint8_t dataByte;
    uint8_t instructionByte;
    void (*write)(uint8_t slaveAddress, uint8_t *data, uint16_t length);
};

AD5280_Handle_T AD5280_Init(void){

    return malloc(sizeof(struct AD5280_Handle_S));

}

void AD5280_ConfigSlaveAddress(AD5280_Handle_T self,
        uint8_t slaveAddress) {

    self->slaveAddress = slaveAddress;

}

void AD5280_ConfigWriteFunction(AD5280_Handle_T self,
        void (*write)(uint8_t slaveAddress, uint8_t *data, uint16_t length)) {

    self->write = write;

}

void AD5280_ConfigNominalResistorValue(AD5280_Handle_T self,
        float nominalResistorValue) {

    self->nominalResistorValue = nominalResistorValue;

}

void AD5280_SetWBResistance(AD5280_Handle_T self,
        float wbResistorValue) {

    self->dataByte = ((uint8_t)(wbResistorValue * 256.0 - 60.0) / 
        self->nominalResistorValue);

    uint8_t data[2] = { self->instructionByte, self->dataByte };

    self->write(self->slaveAddress, data, 2);

}

void AD5280_DeInit(AD5280_Handle_T self) { 

    free(self);

}
