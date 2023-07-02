#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "spi.h"

#include "common.h"
#include "tmc5160.h"
#include "motor.h"
#include "logger.h"

HAL_StatusTypeDef MOTOR_Reset(MOTOR_Handle* const pmotor);
HAL_StatusTypeDef MOTOR_SendCommand(MOTOR_Handle* const pmotor, uint8_t address, uint32_t data);
int32_t MOTOR_ReadData(MOTOR_Handle* const pmotor, uint8_t address);
uint32_t generateIParams(uint8_t irun, uint8_t ihold, uint32_t iholdDelay);

MOTOR_Handle* MOTOR_Init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* csGPIO, uint16_t csPIN, GPIO_TypeDef* limitGPIO, uint16_t limitPIN, float irun, float ihold, float iholdDelay) {
    MOTOR_Handle* pmotor = pvPortMalloc(sizeof(MOTOR_Handle));
    memset(pmotor, 0, sizeof(MOTOR_Handle));
    pmotor->hspi = hspi;
    pmotor->csGPIO = csGPIO;
    pmotor->csPIN = csPIN;
    pmotor->limitGPIO = limitGPIO;
    pmotor->limitPIN = limitPIN;
    pmotor->irun = irun;
    pmotor->ihold = ihold;
    pmotor->iholdDelay = iholdDelay;
    pmotor->stopped = FALSE;
    MOTOR_Reset(pmotor);
    return pmotor;
}

HAL_StatusTypeDef MOTOR_Reset(MOTOR_Handle* const pmotor) {
    uint32_t iparams = generateIParams(pmotor->irun * 10, pmotor->ihold * 10, pmotor->iholdDelay);
    MOTOR_SendCommand(pmotor, TMC5160_CHOPCONF, 0x000100C3);
    MOTOR_SendCommand(pmotor, TMC5160_IHOLD_IRUN, iparams);
    MOTOR_SendCommand(pmotor, TMC5160_TPOWERDOWN, 0x0000000A);
    MOTOR_SendCommand(pmotor, TMC5160_GCONF, 0x00);
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
    if(MOTOR_LimitCheck(pmotor))
        MOTOR_SendCommand(pmotor, TMC5160_SWMODE, 0xF);
    else
        MOTOR_SendCommand(pmotor, TMC5160_SWMODE, 0);
    return MOTOR_SendCommand(pmotor, TMC5160_XTARGET, (int32_t)ceil(142.2 * angle));
}

int16_t MOTOR_GetRotateAngle(MOTOR_Handle* const pmotor) {
    return (int16_t)(MOTOR_ReadData(pmotor, TMC5160_XACTUAL) / 142.2);
}

BOOL MOTOR_LimitCheck(MOTOR_Handle* const pmotor) {
    if(HAL_GPIO_ReadPin(pmotor->limitGPIO, pmotor->limitPIN) == GPIO_PIN_RESET) {
        pmotor->stopped = TRUE;
        return TRUE;
    }
    pmotor->stopped = FALSE;
    return FALSE;
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

uint32_t generateIParams(uint8_t irun, uint8_t ihold, uint32_t iholdDelay) {
	uint8_t data[6];
	data[0] = (iholdDelay >> 24) & 0xFF;
	data[1] = (iholdDelay >> 16) & 0xFF;
	data[2] = (iholdDelay >> 8) & 0xFF;
	data[3] = iholdDelay & 0xFF;
	data[4] = irun;
	data[5] = ihold;
    uint32_t iparams = 0;
    for (uint8_t i = 0; i < 6; i++)
        iparams = (iparams << 8) | (data[i] & 0xFF);
    return iparams;
}

void MOTOR_Free(MOTOR_Handle* pmotor) {
    vPortFree(pmotor);
    pmotor = NULL;
}
