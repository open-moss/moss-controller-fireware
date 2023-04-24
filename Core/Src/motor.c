#include <stdlib.h>
#include "common.h"
#include "tmc5160.h"
#include "motor.h"
#include "spi.h"

MOTOR* new_MOTOR(SPI_HandleTypeDef hspi, GPIO_TypeDef* csGPIO, uint16_t csPIN) {
    MOTOR* pDriver = NULL;
    pDriver = (MOTOR*)malloc(sizeof(MOTOR));
    if(pDriver == NULL)
        return NULL;
    pDriver->hspi = hspi;
    pDriver->csGPIO = csGPIO;
    pDriver->csPIN = csPIN;
    pDriver->init = init;
    pDriver->rotate = rotate;
    pDriver->delete = delete_MOTOR;
    return pDriver;
}

void delete_MOTOR(MOTOR* const pDriver) {
    free(pDriver);
}

void sendCommand(MOTOR* const pDriver, uint8_t address, uint32_t data) {
    uint8_t buffer[5];
    buffer[0] = address | 0x80;
    buffer[1] = 0xFF & (data>>24);
    buffer[2] = 0xFF & (data>>16);
    buffer[3] = 0xFF & (data>>8);
    buffer[4] = 0xFF & data;
    HAL_GPIO_WritePin(pDriver->csGPIO, pDriver->csPIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&pDriver->hspi, buffer, 5, 100);
	HAL_GPIO_WritePin(pDriver->csGPIO, pDriver->csPIN, GPIO_PIN_SET);
}

void init(MOTOR* const pDriver) {
    sendCommand(pDriver, TMC5160_CHOPCONF, 0x000100C3);
    sendCommand(pDriver, TMC5160_IHOLD_IRUN, 0x00060E0A);
    sendCommand(pDriver, TMC5160_TPOWERDOWN, 0x0000000A);
    sendCommand(pDriver, TMC5160_TCOOLTHRS, 0x00000004);
    sendCommand(pDriver, TMC5160_TSTEP, 0x00000004);
    sendCommand(pDriver, TMC5160_GCONF, 0x00000004);
    sendCommand(pDriver, TMC5160_TPWMTHRS, 0x000001F4);
    sendCommand(pDriver, TMC5160_A1, 100);
    sendCommand(pDriver, TMC5160_V1, 500);
    sendCommand(pDriver, TMC5160_AMAX, 50);
    sendCommand(pDriver, TMC5160_VMAX, 20000);
    sendCommand(pDriver, TMC5160_DMAX, 70);
    sendCommand(pDriver, TMC5160_D1, 140);
    sendCommand(pDriver, TMC5160_VSTOP, 10);
    sendCommand(pDriver, TMC5160_RAMPMODE, 0);
}

void rotate(MOTOR* const pDriver, uint32_t angle) {
    sendCommand(pDriver, TMC5160_XTARGET, angle);
}
