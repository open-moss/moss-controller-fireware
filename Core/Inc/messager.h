#ifndef Messager_H
#define Messager_H

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
} Messager_SerialRxBuffer;

typedef struct {
    UART_HandleTypeDef* huart;
    uint8_t tempBufferIndex;
    uint8_t **tempBuffers;
    uint8_t tempBufferCount;
    osMessageQId *messageQueue;
    SemaphoreHandle_t sendSemaphore;
    SemaphoreHandle_t replySemaphore;
    uint16_t waitReplyId;
    Messager_SerialRxBuffer* rxBuffer;
    uint16_t txTimeout;
    uint16_t rxTimeout;
    BOOL rxHeadStart;
} Messager_Handle;

Messager_Handle* Messager_Init(UART_HandleTypeDef *huart, osMessageQId *messageQueue, uint16_t rxBufferSize, uint8_t tempBufferSize, uint16_t txTimeout, uint16_t rxTimeout);
HAL_StatusTypeDef Messager_Listen(Messager_Handle* const pmgr);
HAL_StatusTypeDef Messager_SendMessage(Messager_Handle *const pmgr, DataPacket *const pdata);
HAL_StatusTypeDef Messager_SendMessageWaitReply(Messager_Handle *const pmgr, DataPacket *const pdata, uint16_t replyTimeout);
void Messager_MessageHandle(Messager_Handle* const pmgr);
void Messager_TxCpltCallback(Messager_Handle* const pmgr);
void Messager_RxCpltCallback(Messager_Handle* const pmgr);

#endif
