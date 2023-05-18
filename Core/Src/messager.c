#include "string.h"
#include "usart.h"

#include "common.h"
#include "config.h"
#include "messager.h"

MESSAGER_Handle MESSAGER_Init(UART_HandleTypeDef *huart, uint16_t rxTimeout) {
    SerialRxBuffer rxBuffer;
    rxBuffer.index = 0;
    rxBuffer.size = 0;
    memset(&rxBuffer.temp, 0, sizeof(rxBuffer.temp));  //清空接收缓冲
    memset(&rxBuffer.data, 0, sizeof(rxBuffer.data));  //清空接收缓冲
    MESSAGER_Handle hmessager;
    hmessager.huart = huart;
    hmessager.rxBuffer = &rxBuffer;
    hmessager.rxTimeout = rxTimeout;
    hmessager.serialSending = FALSE;
    hmessager.serialReceiving = FALSE;
    DebugHEXPrint(rxBuffer.temp, 1, __FUNCTION__, __LINE__);
    DebugHEXPrint(rxBuffer.data, UPPER_COMPUTER_SERIAL_PORT_BUFFER_MAX_SIZE, __FUNCTION__, __LINE__);
    return hmessager;
}

HAL_StatusTypeDef MESSAGER_Listen(MESSAGER_Handle* const hmessager) {
    //如果当前串口接收态未处于准备状态则终止上次接收
    if (hmessager->huart->RxState != HAL_UART_STATE_READY)
        HAL_UART_AbortReceive_IT(hmessager->huart);

    hmessager->serialSending = FALSE;
    hmessager->serialReceiving = FALSE;

    uint32_t startTime = SYS_TIME;        //开始时间

    //开始异步接收字符
    while (HAL_UART_Receive_IT(hmessager->huart, hmessager->rxBuffer->temp, 1) != HAL_OK)
    {
        //判断接收是否超时
        if (SYS_TIME - startTime > hmessager->rxTimeout)
        {
            HAL_UART_AbortReceive_IT(hmessager->huart);                     //超时终止接收
            memset(hmessager->rxBuffer, 0, sizeof(SerialRxBuffer));  //清空网络接收缓冲
            return HAL_TIMEOUT;
        }
        osDelay(50);
    }

    return HAL_OK;
}

void MESSAGER_TxCpltCallback(MESSAGER_Handle* const hmessager) {
    hmessager->serialSending = FALSE;
}

void MESSAGER_RxCpltCallback(MESSAGER_Handle* const hmessager) {
    uint32_t startTime = SYS_TIME;  //接收开始时间
    uint8_t c;  //接收的字符
    if(!hmessager->serialReceiving) return;
    c = hmessager->rxBuffer->temp[0];
    DebugPrintfISR("%02X", __FUNCTION__, __LINE__, c);
}
