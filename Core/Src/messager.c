#include "stdlib.h"
#include "string.h"
#include "usart.h"
#include "semphr.h"
#include "cmsis_os.h"

#include "common.h"
#include "config.h"
#include "protocol.h"
#include "messager.h"
#include "logger.h"

extern osMessageQId serialDataQueueHandle;

MESSAGER_Handle *MESSAGER_Init(UART_HandleTypeDef *huart, uint16_t rxBufferSize, uint16_t rxTimeout)
{
    SerialRxBuffer *rxBuffer = pvPortMalloc(sizeof(SerialRxBuffer));
    memset(rxBuffer, 0, sizeof(SerialRxBuffer));
    rxBuffer->data = (uint8_t *)pvPortMalloc(sizeof(uint8_t) * rxBufferSize);
    memset(rxBuffer->data, 0, sizeof(uint8_t) * rxBufferSize);
    rxBuffer->temp = (uint8_t *)pvPortMalloc(sizeof(uint8_t));
    memset(rxBuffer->temp, 0, sizeof(uint8_t));
    rxBuffer->size = rxBufferSize;
    rxBuffer->receiveSize = 0;
    MESSAGER_Handle *pmgr = pvPortMalloc(sizeof(MESSAGER_Handle));
    memset(pmgr, 0, sizeof(MESSAGER_Handle));
    pmgr->huart = huart;
    pmgr->rxBuffer = rxBuffer;
    pmgr->rxTimeout = rxTimeout;
    pmgr->serialSendSemaphoreHandle = xSemaphoreCreateMutex();
    pmgr->serialSending = FALSE;
    pmgr->serialReceiving = FALSE;
    return pmgr;
}

HAL_StatusTypeDef MESSAGER_Listen(MESSAGER_Handle *const pmgr)
{
    // 如果当前串口接收态未处于准备状态则终止上次接收
    if (pmgr->huart->RxState != HAL_UART_STATE_READY)
        HAL_UART_AbortReceive_IT(pmgr->huart);

    pmgr->serialSending = FALSE;
    pmgr->serialReceiving = FALSE;

    uint32_t startTime = SYS_TIME; // 开始时间

    // 开始异步接收字符
    while (HAL_UART_Receive_IT(pmgr->huart, pmgr->rxBuffer->temp, 1) != HAL_OK)
    {
        // 判断接收是否超时
        if (SYS_TIME - startTime > pmgr->rxTimeout)
        {
            HAL_UART_AbortReceive_IT(pmgr->huart);             // 超时终止接收
            memset(pmgr->rxBuffer, 0, sizeof(SerialRxBuffer)); // 清空网络接收缓冲
            return HAL_TIMEOUT;
        }
        osDelay(50);
    }

    return HAL_OK;
}

void MESSAGER_RxBufferClear(MESSAGER_Handle *const pmgr)
{
    memset(pmgr->rxBuffer->data, 0, sizeof(uint8_t) * pmgr->rxBuffer->size);
    memset(pmgr->rxBuffer->temp, 0, sizeof(uint8_t));
    pmgr->rxBuffer->receiveSize = 0;
    pmgr->rxBuffer->index = 0;
}

void MESSAGER_TxCpltCallback(MESSAGER_Handle *const pmgr)
{
    pmgr->serialSending = FALSE;
}

void MESSAGER_RxCpltCallback(MESSAGER_Handle *const pmgr)
{
    uint32_t startTime = SYS_TIME; // 接收开始时间
    SerialRxBuffer *rxBuffer = pmgr->rxBuffer;
    uint8_t c = rxBuffer->temp[0]; // 接收的字符
    if (c == 0x0D && rxBuffer->data[0] != 0x00)
    {
        if (rxBuffer->index + 1 > rxBuffer->size)
            MESSAGER_RxBufferClear(pmgr);
        if (rxBuffer->receiveSize == 0 && rxBuffer->index + 1 >= 3)
            rxBuffer->receiveSize = MergeToUint16(rxBuffer->data[1], rxBuffer->data[2]); // 从包头获取数据大小
        if (rxBuffer->index + 1 < rxBuffer->receiveSize)
            rxBuffer->data[rxBuffer->index++] = c; // 追加字符
        else if (rxBuffer->index + 1 >= DATA_PACKET_MIN_SIZE)
        {
            rxBuffer->data[rxBuffer->index++] = c;
            DataPacket *pData = Protocol_BufferToDataPacket(rxBuffer->data);
            MESSAGER_RxBufferClear(pmgr);
            osMessagePut(serialDataQueueHandle, (uint32_t)pData, 100); // 推送数据包到消息队列
        }
    }
    else
        rxBuffer->data[rxBuffer->index++] = c;
    // 开始接收下一个字符
    while (HAL_UART_Receive_IT(pmgr->huart, pmgr->rxBuffer->temp, 1) != HAL_OK)
    {
        // 判断接收是否超时
        if (SYS_TIME - startTime > pmgr->rxTimeout)
        {
            HAL_UART_AbortReceive_IT(pmgr->huart);  //超时终止接收
            MESSAGER_RxBufferClear(pmgr);
            return;
        }
        osDelay(50);
    }
    PrintHEX(rxBuffer->temp, 1);
}
