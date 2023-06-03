#include "stdlib.h"
#include "string.h"
#include "usart.h"
#include "semphr.h"
#include "cmsis_os.h"

#include "common.h"
#include "config.h"
#include "protocol.h"
#include "logger.h"
#include "messager.h"
#include "logger.h"

void MESSAGER_RxBufferClear(MESSAGER_Handle *const pmgr);

MESSAGER_Handle *MESSAGER_Init(UART_HandleTypeDef *huart, osMessageQId *messageQueue, uint16_t rxBufferSize, uint16_t txTimeout, uint16_t rxTimeout)
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
    pmgr->messageQueue = messageQueue;
    pmgr->rxBuffer = rxBuffer;
    pmgr->waitReplyId = 0;
    pmgr->txTimeout = txTimeout;
    pmgr->rxTimeout = rxTimeout;
    pmgr->sendSemaphore = xSemaphoreCreateMutex();
    pmgr->replySemaphore = xSemaphoreCreateBinary();
    pmgr->rxHeadStart = FALSE;
    pmgr->tempBuffers = (uint8_t**)pvPortMalloc(sizeof(uint8_t*) * 5);
    for(uint8_t i = 0;i < 5;i++) {
        pmgr->tempBuffers[i] = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * rxBufferSize);
        memset(pmgr->tempBuffers[i], 0, sizeof(uint8_t) * rxBufferSize);
    }
    return pmgr;
}

HAL_StatusTypeDef MESSAGER_Listen(MESSAGER_Handle *const pmgr)
{
    // 如果当前串口接收态未处于准备状态则终止上次接收
    if (pmgr->huart->RxState != HAL_UART_STATE_READY)
        HAL_UART_AbortReceive_IT(pmgr->huart);

    pmgr->rxHeadStart = FALSE;

    uint32_t startTime = SYS_TIME; // 开始时间

    // 开始异步接收字符
    while (HAL_UART_Receive_IT(pmgr->huart, pmgr->rxBuffer->temp, 1) != HAL_OK)
    {
        // 判断接收是否超时
        if (SYS_TIME - startTime > pmgr->rxTimeout)
        {
            HAL_UART_AbortReceive_IT(pmgr->huart);
            MESSAGER_RxBufferClear(pmgr);
            return HAL_TIMEOUT;
        }
        osDelay(50);
    }

    return HAL_OK;
}

void MESSAGER_MessageHandle(MESSAGER_Handle *const pmgr) {
    osEvent event = osMessageGet(pmgr->messageQueue, 1000); //消息队列接收消息
    if (event.status != osEventMessage)
        return;
    DataPacket *pdata = Protocol_BufferToDataPacket(event.value.p);
    if(pmgr->waitReplyId == pdata->id) {
        pmgr->waitReplyId = 0;
        xSemaphoreGive(pmgr->replySemaphore);
    }
    Protocol_PrintDataPacket(pdata);
    Protocol_FreeDataPacket(pdata);
}

HAL_StatusTypeDef MESSAGER_SendMessage(MESSAGER_Handle *const pmgr, DataPacket *const pdata) {
    xSemaphoreTake(pmgr->sendSemaphore, 50);
    uint32_t startTime = SYS_TIME;
    uint8_t *buffer = Protocol_DataPacketToBuffer(pdata);
    while (HAL_UART_Transmit_IT(pmgr->huart, buffer, pdata->size) != HAL_OK)
    {
        //判断发送开启超时
        if (SYS_TIME - startTime > pmgr->txTimeout)
        {
            HAL_UART_AbortTransmit_IT(pmgr->huart);  //超时终止发送
            xSemaphoreGive(pmgr->sendSemaphore);
            return HAL_TIMEOUT;
        }
        osDelay(50);
    }
    Protocol_FreeBuffer(buffer);
    xSemaphoreGive(pmgr->sendSemaphore);
    return HAL_OK;
}

HAL_StatusTypeDef MESSAGER_SendMessageWaitReply(MESSAGER_Handle *const pmgr, DataPacket *const pdata, uint16_t replyTimeout) {
    xSemaphoreTake(pmgr->sendSemaphore, 50);
    uint32_t startTime = SYS_TIME;
    uint8_t *buffer = Protocol_DataPacketToBuffer(pdata);
    pmgr->waitReplyId = pdata->id;
    while (HAL_UART_Transmit_IT(pmgr->huart, buffer, pdata->size) != HAL_OK)
    {
        //判断发送开启超时
        if (SYS_TIME - startTime > pmgr->txTimeout)
        {
            HAL_UART_AbortTransmit_IT(pmgr->huart);  //超时终止发送
            xSemaphoreGive(pmgr->sendSemaphore);
            return HAL_TIMEOUT;
        }
        osDelay(50);
    }
    Protocol_FreeBuffer(buffer);
    if (xSemaphoreTake(pmgr->replySemaphore, replyTimeout) != pdTRUE) {
        xSemaphoreGive(pmgr->sendSemaphore);
        return HAL_TIMEOUT;
    }
    xSemaphoreGive(pmgr->sendSemaphore);
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
    
}

void MESSAGER_RxCpltCallback(MESSAGER_Handle *const pmgr)
{
    uint32_t startTime = SYS_TIME; // 接收开始时间
    SerialRxBuffer *rxBuffer = pmgr->rxBuffer;
    uint8_t c = rxBuffer->temp[0]; // 接收的字符
    if(pmgr->rxHeadStart && rxBuffer->index == 1 && c != DATA_PACKET_HEAD_LOW) {
        pmgr->rxHeadStart = FALSE;
        MESSAGER_RxBufferClear(pmgr);
    }
    if(!pmgr->rxHeadStart && c == DATA_PACKET_HEAD_HIGH) {
        pmgr->rxHeadStart = TRUE;
        MESSAGER_RxBufferClear(pmgr);
    }
    if (pmgr->rxHeadStart && c == 0x0D && rxBuffer->data[0] != 0x00)
    {
        if (rxBuffer->receiveSize == 0 && rxBuffer->index + 1 >= 5) {
            rxBuffer->receiveSize = MergeToUint16(rxBuffer->data[3], rxBuffer->data[4]); // 从包头获取数据大小
        }
        if (rxBuffer->index + 1 < rxBuffer->receiveSize)
            rxBuffer->data[rxBuffer->index++] = c; // 追加字符
        else if (rxBuffer->index + 1 >= DATA_PACKET_MIN_SIZE)
        {
            rxBuffer->data[rxBuffer->index++] = c;
            memcpy(pmgr->tempBuffers[0], rxBuffer->data, rxBuffer->size);
            pmgr->rxHeadStart = FALSE;
            MESSAGER_RxBufferClear(pmgr);
            osMessagePut(pmgr->messageQueue, (uint32_t)pmgr->tempBuffers[0], 100); // 推送数据包到消息队列
        }
    }
    else
        rxBuffer->data[rxBuffer->index++] = c;
    if (rxBuffer->index + 1 > rxBuffer->size)
        MESSAGER_RxBufferClear(pmgr);
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
}
