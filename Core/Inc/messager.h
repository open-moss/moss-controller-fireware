#ifndef MESSAGER_H
#define MESSAGER_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"

#include "common.h"
#include "config.h"
#include "protocol.h"

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
    uint8_t tempBufferIndex;
    uint8_t **tempBuffers;
    uint8_t tempBufferCount;
    osMessageQId *messageQueue;
    SemaphoreHandle_t sendSemaphore;
    SemaphoreHandle_t replySemaphore;
    uint16_t waitReplyId;
    uint16_t txTimeout;
    uint16_t rxTimeout;
    BOOL rxHeadStart;
} MESSAGER_Handle;

MESSAGER_Handle* MESSAGER_Init(UART_HandleTypeDef *huart, osMessageQId *messageQueue, uint16_t rxBufferSize, uint8_t tempBufferSize, uint16_t txTimeout, uint16_t rxTimeout);
HAL_StatusTypeDef MESSAGER_Listen(MESSAGER_Handle* const pmgr);
HAL_StatusTypeDef MESSAGER_SendMessage(MESSAGER_Handle *const pmgr, DataPacket *const pdata);
HAL_StatusTypeDef MESSAGER_SendMessageWaitReply(MESSAGER_Handle *const pmgr, DataPacket *const pdata, uint16_t replyTimeout);
void MESSAGER_MessageHandle(MESSAGER_Handle* const pmgr);
void MESSAGER_TxCpltCallback(MESSAGER_Handle* const pmgr);
void MESSAGER_RxCpltCallback(MESSAGER_Handle* const pmgr);

#endif
