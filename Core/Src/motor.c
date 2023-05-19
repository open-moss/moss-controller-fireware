#include "stdlib.h"
#include "math.h"

#include "common.h"
#include "tmc5160.h"
#include "motor.h"
#include "spi.h"

HAL_StatusTypeDef MOTOR_Reset(MOTOR_Handle* const pmotor);
HAL_StatusTypeDef MOTOR_SendCommand(MOTOR_Handle* const pmotor, uint8_t address, uint32_t data);
int32_t MOTOR_ReadData(MOTOR_Handle* const pmotor, uint8_t address);

MOTOR_Handle* MOTOR_Init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* csGPIO, uint16_t csPIN) {
    MOTOR_Handle* pmotor = pvPortMalloc(sizeof(MOTOR_Handle));
    pmotor->hspi = hspi;
    pmotor->csGPIO = csGPIO;
    pmotor->csPIN = csPIN;
    MOTOR_Reset(pmotor);
    return pmotor;
}

HAL_StatusTypeDef MOTOR_Reset(MOTOR_Handle* const pmotor) {
    MOTOR_SendCommand(pmotor, TMC5160_CHOPCONF, 0x000100C3);
    MOTOR_SendCommand(pmotor, TMC5160_IHOLD_IRUN, 0x00060303);
    MOTOR_SendCommand(pmotor, TMC5160_TPOWERDOWN, 0x0000000A);
    MOTOR_SendCommand(pmotor, TMC5160_GCONF, 0x00000004);
    MOTOR_SendCommand(pmotor, TMC5160_TPWMTHRS, 0x000001F4);
    MOTOR_SendCommand(pmotor, TMC5160_A1, 100);
    MOTOR_SendCommand(pmotor, TMC5160_V1, 500);
    MOTOR_SendCommand(pmotor, TMC5160_AMAX, 50);
    MOTOR_SendCommand(pmotor, TMC5160_VMAX, 20000);
    MOTOR_SendCommand(pmotor, TMC5160_DMAX, 70);
    MOTOR_SendCommand(pmotor, TMC5160_D1, 140);
    MOTOR_SendCommand(pmotor, TMC5160_VSTOP, 10);
    MOTOR_SendCommand(pmotor, TMC5160_RAMPMODE, 0);
    MOTOR_SendCommand(pmotor, TMC5160_XACTUAL, 0);
    return HAL_OK;
}

HAL_StatusTypeDef MOTOR_Rotate(MOTOR_Handle* const pmotor, int16_t angle) {
    return MOTOR_SendCommand(pmotor, TMC5160_XTARGET, (int32_t)ceil(142.2 * angle));
}

int16_t MOTOR_GetRotateAngle(MOTOR_Handle* const pmotor) {
    return (int16_t)(MOTOR_ReadData(pmotor, TMC5160_XACTUAL) / 142.2);
}

HAL_StatusTypeDef MOTOR_SendCommand(MOTOR_Handle* const pmotor, uint8_t address, uint32_t data) {
    uint8_t buffer[5];
    buffer[0] = address | 0x80;
    buffer[1] = 0xFF & (data>>24);
    buffer[2] = 0xFF & (data>>16);
    buffer[3] = 0xFF & (data>>8);
    buffer[4] = 0xFF & data;
    HAL_GPIO_WritePin(pmotor->csGPIO, pmotor->csPIN, GPIO_PIN_RESET);
    HAL_StatusTypeDef ret = HAL_SPI_Transmit(pmotor->hspi, buffer, 5, 100);
	HAL_GPIO_WritePin(pmotor->csGPIO, pmotor->csPIN, GPIO_PIN_SET);
    return ret;
}

int32_t MOTOR_ReadData(MOTOR_Handle* const pmotor, uint8_t address) {
    uint8_t txBuffer[5] = { address, 0x00, 0x00, 0x00, 0x00 };
    uint8_t rxBuffer[5] = { 0 };
    HAL_GPIO_WritePin(pmotor->csGPIO, pmotor->csPIN, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(pmotor->hspi, txBuffer, rxBuffer, 5, 100);
	HAL_GPIO_WritePin(pmotor->csGPIO, pmotor->csPIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(pmotor->csGPIO, pmotor->csPIN, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(pmotor->hspi, txBuffer, rxBuffer, 5, 100);
	HAL_GPIO_WritePin(pmotor->csGPIO, pmotor->csPIN, GPIO_PIN_SET);
    int32_t ret = (rxBuffer[1] << 24) | (rxBuffer[2] << 16) | (rxBuffer[3] << 8) | rxBuffer[4];
    return ret;
}
