#ifndef BodySensor_H
#define BodySensor_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"

#include "common.h"

#define BODY_SENSOR_DATA_HEAD { 0xAA, 0xFF, 0x03, 0x00 }
#define BODY_SENSOR_DATA_BODY_SIZE 24

typedef struct {
    uint8_t* data;  //接收字符数组
    uint8_t* temp;  //接收字符暂存区
    uint8_t index;  //数据位置指针
} BodySensor_SerialRxBuffer;

typedef struct {
    UART_HandleTypeDef* huart;
    osMessageQId *messageQueue;
    uint8_t tempBufferIndex;
    uint8_t **tempBuffers;
    uint8_t tempBufferCount;
    BodySensor_SerialRxBuffer* rxBuffer;
    uint16_t rxTimeout;
    BOOL rxHeadStart;
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

BodySensor_Handle* BodySensor_Init(UART_HandleTypeDef* huart, osMessageQId *messageQueue, uint8_t tempBufferCount, uint16_t rxTimeout);
HAL_StatusTypeDef BodySensor_Listen(BodySensor_Handle *const pbs);
BodySensor_MeasureData* BodySensor_Measuring(BodySensor_Handle *const pbs);
void BodySensor_FreeMeasureData(BodySensor_MeasureData *measureData);
void BodySensor_RxCpltCallback(BodySensor_Handle *pbs);

#endif
