#ifndef MOTOR_H
#define MOTOR_H

#include "common.h"
#include "tmc5160.h"

typedef struct _MOTOR_Handle MOTOR_Handle;
typedef struct _MOTOR_Handle {
    SPI_HandleTypeDef* hspi;
    GPIO_TypeDef* csGPIO;
    uint16_t csPIN;
}MOTOR_Handle;

MOTOR_Handle MOTOR_Init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* csGPIO, uint16_t csPIN);
HAL_StatusTypeDef MOTOR_Reset(MOTOR_Handle* const hmotor);
HAL_StatusTypeDef MOTOR_Rotate(MOTOR_Handle* const hmotor, int16_t angle);
int16_t MOTOR_GetRotateAngle(MOTOR_Handle* const hmotor);
HAL_StatusTypeDef MOTOR_SendCommand(MOTOR_Handle* const hmotor, uint8_t address, uint32_t data);
int32_t MOTOR_ReadData(MOTOR_Handle* const hmotor, uint8_t address);

#endif
