#ifndef BodySensor_H
#define BodySensor_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"

typedef struct {
    UART_HandleTypeDef* huart;
} BodySensor_Handle;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t speed;
    uint16_t distance;
} BodySensor_BodyData;

typedef struct {
    BodySensor_BodyData *body1;
    BodySensor_BodyData *body2;
    BodySensor_BodyData *body3;
} BodySensor_MeasureData;

BodySensor_Handle* BodySensor_Init(UART_HandleTypeDef* huart);
BodySensor_MeasureData* BodySensor_Measuring(BodySensor_Handle *pbs);
void BodySensor_FreeMeasureData(BodySensor_MeasureData *measureData);

#endif
