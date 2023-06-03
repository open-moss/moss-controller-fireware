#ifndef MESSAGER_H
#define MESSAGER_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"

#include "common.h"
#include "config.h"

typedef struct {
    uint8_t* data;  //接收字符数组
    uint8_t* temp;  //接收字符暂存区
    uint8_t index;  //数据位置指针
    uint16_t size;  //数据最大大小
    uint16_t receiveSize;  //数据接收大小
} SerialRxBuffer;

typedef struct {
    UART_HandleTypeDef* huart;
    SerialRxBuffer* rxBuffer;
    uint8_t **tempBuffers;
    SemaphoreHandle_t serialSendSemaphoreHandle;
    uint16_t rxTimeout;
    BOOL rxHeadStart;
    BOOL serialSending;
    BOOL serialReceiving;
} MESSAGER_Handle;

MESSAGER_Handle* MESSAGER_Init(UART_HandleTypeDef *huart, uint16_t rxBufferSize, uint16_t rxTimeout);
HAL_StatusTypeDef MESSAGER_Listen(MESSAGER_Handle* const hmessager);
void MESSAGER_MessageHandle(void);
void MESSAGER_TxCpltCallback(MESSAGER_Handle* const hmessager);
void MESSAGER_RxCpltCallback(MESSAGER_Handle* const hmessager);

#endif
