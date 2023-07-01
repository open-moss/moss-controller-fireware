#ifndef BodySensor_H
#define BodySensor_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"

typedef struct {
    uint8_t* data;  //接收字符数组
    uint8_t* temp;  //接收字符暂存区
    uint8_t index;  //数据位置指针
    uint16_t size;  //数据最大大小
    uint16_t receiveSize;  //数据接收大小
} BodySensor_SerialRxBuffer;

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
void BodySensor_RxCpltCallback(BodySensor_Handle *pbs);

#endif
