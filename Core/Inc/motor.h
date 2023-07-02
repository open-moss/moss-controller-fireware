#ifndef MOTOR_H
#define MOTOR_H

#include "common.h"
#include "tmc5160.h"

typedef struct {
    SPI_HandleTypeDef* hspi;
    GPIO_TypeDef* csGPIO;
    uint16_t csPIN;
    GPIO_TypeDef* limitGPIO;
    uint16_t limitPIN;
    float ihold;
    float irun;
    float iholdDelay;
    BOOL locked;
} MOTOR_Handle;

MOTOR_Handle* MOTOR_Init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* csGPIO, uint16_t csPIN, GPIO_TypeDef* limitGPIO, uint16_t limitPIN, float irun, float ihold, float iholdDelay);
HAL_StatusTypeDef MOTOR_Reset(MOTOR_Handle* const hmotor);
HAL_StatusTypeDef MOTOR_Rotate(MOTOR_Handle* const hmotor, int16_t angle);
HAL_StatusTypeDef MOTOR_Unlock(MOTOR_Handle* const pmotor);
HAL_StatusTypeDef MOTOR_Lock(MOTOR_Handle* const pmotor);
int16_t MOTOR_GetRotateAngle(MOTOR_Handle* const hmotor);
BOOL MOTOR_LimitCheck(MOTOR_Handle* const pmotor);
HAL_StatusTypeDef MOTOR_SendCommand(MOTOR_Handle* const hmotor, uint8_t address, uint32_t data);
int32_t MOTOR_ReadData(MOTOR_Handle* const hmotor, uint8_t address);
void MOTOR_Free(MOTOR_Handle* const pmotor);

#endif
