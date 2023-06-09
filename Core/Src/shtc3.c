#include "shtc3.h"

void SHTC3_WriteByte(I2C_HandleTypeDef* hi2c, uint16_t data);

void SHTC3_Wakeup(I2C_HandleTypeDef* hi2c) {
    SHTC3_WriteByte(hi2c, SHTC3_COMMAND_WAKEUP);
}

void SHTC3_Measuring(I2C_HandleTypeDef* hi2c) {
    SHTC3_WriteByte(hi2c, SHTC3_COMMAND_MEASURING);
}

void SHTC3_Read(I2C_HandleTypeDef* hi2c, uint8_t* measureData) {

    HAL_I2C_Master_Receive(hi2c, SHTC3_ADDRESS_READ, measureData, sizeof(uint8_t) * 6, 1000);
}

void SHTC3_WriteByte(I2C_HandleTypeDef* hi2c, uint16_t data) {
    HAL_I2C_Master_Transmit(hi2c, SHTC3_ADDRESS_WRITE, (uint8_t *){ ExtractUint8High(data), ExtractUint8Low(data) }, sizeof(uint8_t) * 2, 1000);
    osDelay(10);
}
