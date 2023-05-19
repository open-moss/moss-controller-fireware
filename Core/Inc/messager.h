#ifndef MESSAGER_H
#define MESSAGER_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"

#include "common.h"
#include "config.h"

typedef struct {
    uint8_t data[UPPER_COMPUTER_SERIAL_PORT_BUFFER_MAX_SIZE];  //接收字符数组
    uint8_t temp[1];  //接收字符暂存区
    uint8_t index;  //数据位置指针
    uint16_t size;  //数据大小
} SerialRxBuffer;

typedef struct {
    UART_HandleTypeDef* huart;
    SerialRxBuffer* rxBuffer;
    uint16_t rxTimeout;
    BOOL serialSending;
    BOOL serialReceiving;
} MESSAGER_Handle;

MESSAGER_Handle* MESSAGER_Init(UART_HandleTypeDef *huart, uint16_t rxTimeout);
HAL_StatusTypeDef MESSAGER_Listen(MESSAGER_Handle* const hmessager);
void MESSAGER_TxCpltCallback(MESSAGER_Handle* const hmessager);
void MESSAGER_RxCpltCallback(MESSAGER_Handle* const hmessager);

#endif
