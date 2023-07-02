#include "string.h"
#include "usart.h"

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include "body_sensor.h"
#include "logger.h"

void BodySensor_RxBufferClear(BodySensor_Handle *const pbs);

BodySensor_Handle* BodySensor_Init(UART_HandleTypeDef* huart, osMessageQId *messageQueue, uint8_t tempBufferCount, uint16_t rxTimeout) {
    BodySensor_SerialRxBuffer *rxBuffer = pvPortMalloc(sizeof(BodySensor_SerialRxBuffer));
    memset(rxBuffer, 0, sizeof(BodySensor_SerialRxBuffer));
    rxBuffer->data = (uint8_t *)pvPortMalloc(sizeof(uint8_t) * BODY_SENSOR_DATA_BODY_SIZE);
    memset(rxBuffer->data, 0, sizeof(uint8_t) * BODY_SENSOR_DATA_BODY_SIZE);
    rxBuffer->temp = (uint8_t *)pvPortMalloc(sizeof(uint8_t));
    memset(rxBuffer->temp, 0, sizeof(uint8_t));
    BodySensor_Handle *pbs = pvPortMalloc(sizeof(BodySensor_Handle));
    memset(pbs, 0, sizeof(BodySensor_Handle));
    pbs->huart = huart;
    pbs->messageQueue = messageQueue;
    pbs->rxBuffer = rxBuffer;
    pbs->rxTimeout = rxTimeout;
    pbs->rxHeadStart = FALSE;
    pbs->tempBufferIndex = 0;
    pbs->tempBufferCount = tempBufferCount;
    pbs->tempBuffers = (uint8_t**)pvPortMalloc(sizeof(uint8_t*) * tempBufferCount);
    for(uint8_t i = 0;i < tempBufferCount;i++) {
        pbs->tempBuffers[i] = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * BODY_SENSOR_DATA_BODY_SIZE);
        memset(pbs->tempBuffers[i], 0, sizeof(uint8_t) * BODY_SENSOR_DATA_BODY_SIZE);
    }
    return pbs;
}

HAL_StatusTypeDef BodySensor_Listen(BodySensor_Handle *const pbs) {
    // 如果当前串口接收态未处于准备状态则终止上次接收
    if (pbs->huart->RxState != HAL_UART_STATE_READY)
        HAL_UART_AbortReceive_IT(pbs->huart);

    pbs->rxHeadStart = FALSE;

    uint32_t startTime = SYS_TIME; // 开始时间

    // 开始异步接收字符
    while (HAL_UART_Receive_IT(pbs->huart, pbs->rxBuffer->temp, 1) != HAL_OK)
    {
        // 判断接收是否超时
        if (SYS_TIME - startTime > pbs->rxTimeout)
        {
            HAL_UART_AbortReceive_IT(pbs->huart);
            BodySensor_RxBufferClear(pbs);
            return HAL_TIMEOUT;
        }
        osDelay(50);
    }

    return HAL_OK;
}

BodySensor_MeasureData* BodySensor_Measuring(BodySensor_Handle *const pbs) {
    osEvent event = osMessageGet(pbs->messageQueue, 200);  //消息队列接收消息
    if (event.status != osEventMessage)
        return NULL;
    uint8_t *pdata = event.value.p;
    BodySensor_Target *target1 = pvPortMalloc(sizeof(BodySensor_Target));
    memset(target1, 0, sizeof(BodySensor_Target));
    BodySensor_Target *target2 = pvPortMalloc(sizeof(BodySensor_Target));
    memset(target2, 0, sizeof(BodySensor_Target));
    BodySensor_Target *target3 = pvPortMalloc(sizeof(BodySensor_Target));
    memset(target3, 0, sizeof(BodySensor_Target));
    BodySensor_MeasureData *measureData = pvPortMalloc(sizeof(BodySensor_MeasureData));
    measureData->target1 = target1;
    measureData->target2 = target2;
    measureData->target3 = target3;
    target1->exists = pdata[0] != 0 && pdata[1] != 0 && pdata[2] != 0 && pdata[3] != 0;
    if(target1->exists) {
        target1->x = 0 - (pdata[0] + pdata[1] * 256);
        target1->y = (pdata[2] + pdata[3] * 256) - 32768;
        target1->speed = 0 - (pdata[4] + pdata[5] * 256);
        target1->distance = pdata[6] + pdata[7] * 256;
    }
    target2->exists = pdata[8] != 0 && pdata[9] != 0 && pdata[10] != 0 && pdata[11] != 0;
    if(target2->exists) {
        target2->x = 0 - (pdata[8] + pdata[9] * 256);
        target2->y = (pdata[10] + pdata[11] * 256) - 32768;
        target2->speed = 0 - (pdata[12] + pdata[13] * 256);
        target2->distance = pdata[14] + pdata[15] * 256;
    }
    target3->exists = pdata[16] != 0 && pdata[17] != 0 && pdata[18] != 0 && pdata[19] != 0;
    if(target3->exists) {
        target3->x = 0 - (pdata[16] + pdata[17] * 256);
        target3->y = (pdata[18] + pdata[19] * 256) - 32768;
        target3->speed = 0 - (pdata[20] + pdata[21] * 256);
        target3->distance = pdata[22] + pdata[23] * 256;
    }
    return measureData;
}

uint8_t* BodySensor_GetTempBufferPoint(BodySensor_Handle *const pbs) {
    if(pbs->tempBufferIndex >= pbs->tempBufferCount)
        pbs->tempBufferIndex = 0;
    memset(pbs->tempBuffers[pbs->tempBufferIndex], 0, sizeof(uint8_t) * BODY_SENSOR_DATA_BODY_SIZE);
    return pbs->tempBuffers[pbs->tempBufferIndex++];
}

void BodySensor_RxBufferClear(BodySensor_Handle *const pbs) {
    memset(pbs->rxBuffer->data, 0, sizeof(uint8_t) * BODY_SENSOR_DATA_BODY_SIZE);
    memset(pbs->rxBuffer->temp, 0, sizeof(uint8_t));
    pbs->rxBuffer->index = 0;
}

void BodySensor_RxCpltCallback(BodySensor_Handle *pbs) {
    static uint8_t head[] = BODY_SENSOR_DATA_HEAD;
    static uint8_t headIndex = 0;
    uint32_t startTime = SYS_TIME; // 接收开始时间
    BodySensor_SerialRxBuffer *rxBuffer = pbs->rxBuffer;
    uint8_t c = rxBuffer->temp[0]; // 接收的字符
    if(!pbs->rxHeadStart) {
        if(c == head[headIndex])
            headIndex++;
        else
            headIndex = 0;
        if(headIndex == sizeof(head) / sizeof(uint8_t)) {
            headIndex = 0;
            pbs->rxHeadStart = TRUE;
            BodySensor_RxBufferClear(pbs);
        }
        if (rxBuffer->index + 1 > BODY_SENSOR_DATA_BODY_SIZE)
            BodySensor_RxBufferClear(pbs);
    }
    else {
        if (rxBuffer->index + 1 < BODY_SENSOR_DATA_BODY_SIZE)
            rxBuffer->data[rxBuffer->index++] = c; // 追加字符
        else
        {
            rxBuffer->data[rxBuffer->index++] = c;
            uint8_t *tempBuffer = BodySensor_GetTempBufferPoint(pbs);
            memcpy(tempBuffer, rxBuffer->data, BODY_SENSOR_DATA_BODY_SIZE);
            pbs->rxHeadStart = FALSE;
            BodySensor_RxBufferClear(pbs);
            osMessagePut(pbs->messageQueue, (uint32_t)tempBuffer, 100); // 推送数据包到消息队列
        }
    }
    // 开始接收下一个字符
    while (HAL_UART_Receive_IT(pbs->huart, pbs->rxBuffer->temp, 1) != HAL_OK)
    {
        // 判断接收是否超时
        if (SYS_TIME - startTime > pbs->rxTimeout)
        {
            HAL_UART_AbortReceive_IT(pbs->huart);  //超时终止接收
            BodySensor_RxBufferClear(pbs);
            return;
        }
        osDelay(50);
    }
}

void BodySensor_FreeMeasureData(BodySensor_MeasureData *measureData) {
    if(measureData == NULL) return;
    vPortFree(measureData->target1);
    measureData->target1 = NULL;
    vPortFree(measureData->target2);
    measureData->target2 = NULL;
    vPortFree(measureData->target3);
    measureData->target3 = NULL;
    vPortFree(measureData);
    measureData = NULL;
}
