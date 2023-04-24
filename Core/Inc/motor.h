#include "common.h"
#include "tmc5160.h"

#ifndef MOTOR_H
#define MOTOR_H

typedef struct _MOTOR MOTOR;

typedef struct _MOTOR {
    SPI_HandleTypeDef hspi;
    GPIO_TypeDef* csGPIO;
    uint16_t csPIN;
    void (*init)(MOTOR* const pDriver);
    void (*rotate)(MOTOR* const pDriver, uint32_t angle);
    void* delete;
}MOTOR;

MOTOR* new_MOTOR(SPI_HandleTypeDef hspi, GPIO_TypeDef* csGPIO, uint16_t csPIN);
void delete_MOTOR(MOTOR* const pDriver);
void init(MOTOR* const pDriver);
void rotate(MOTOR* const pDriver, uint32_t angle);

#endif
