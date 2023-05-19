#include "stdlib.h"
#include "string.h"
#include "usart.h"

#include "common.h"
#include "config.h"
#include "messager.h"

MESSAGER_Handle* MESSAGER_Init(UART_HandleTypeDef *huart, uint16_t rxTimeout) {
    SerialRxBuffer *rxBuffer = pvPortMalloc(sizeof(SerialRxBuffer));
    memset(rxBuffer, 0, sizeof(SerialRxBuffer));  //清空接收缓冲
    MESSAGER_Handle *pmgr = pvPortMalloc(sizeof(MESSAGER_Handle));
    memset(pmgr, 0, sizeof(MESSAGER_Handle));  //清空接收缓冲
    pmgr->huart = huart;
    pmgr->rxBuffer = rxBuffer;
    pmgr->rxTimeout = rxTimeout;
    pmgr->serialSending = FALSE;
    pmgr->serialReceiving = FALSE;
    return pmgr;
}

HAL_StatusTypeDef MESSAGER_Listen(MESSAGER_Handle* const pmgr) {
    //如果当前串口接收态未处于准备状态则终止上次接收
    if (pmgr->huart->RxState != HAL_UART_STATE_READY)
        HAL_UART_AbortReceive_IT(pmgr->huart);

    pmgr->serialSending = FALSE;
    pmgr->serialReceiving = FALSE;

    uint32_t startTime = SYS_TIME;        //开始时间
    DebugHEXPrint(pmgr->rxBuffer->temp, 1, __FUNCTION__, __LINE__);
    //开始异步接收字符
    while (HAL_UART_Receive_IT(pmgr->huart, pmgr->rxBuffer->temp, 1) != HAL_OK)
    {
        //判断接收是否超时
        if (SYS_TIME - startTime > pmgr->rxTimeout)
        {
            HAL_UART_AbortReceive_IT(pmgr->huart);                     //超时终止接收
            memset(pmgr->rxBuffer, 0, sizeof(SerialRxBuffer));  //清空网络接收缓冲
            return HAL_TIMEOUT;
        }
        osDelay(50);
    }

    return HAL_OK;
}

void MESSAGER_TxCpltCallback(MESSAGER_Handle* const pmgr) {
    pmgr->serialSending = FALSE;
}

void MESSAGER_RxCpltCallback(MESSAGER_Handle* const pmgr) {
    uint32_t startTime = SYS_TIME;  //接收开始时间
    uint8_t c;  //接收的字符
    // if(!pmgr->serialReceiving) return;
    c = pmgr->rxBuffer->temp[0];
    DebugPrintfISR("%02X", __FUNCTION__, __LINE__, c);
}
