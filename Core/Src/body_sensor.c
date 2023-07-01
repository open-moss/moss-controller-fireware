#include "string.h"

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include "body_sensor.h"
#include "logger.h"

BodySensor_Handle* BodySensor_Init(UART_HandleTypeDef* huart) {
    BodySensor_Handle *pbs = pvPortMalloc(sizeof(BodySensor_Handle));
    memset(pbs, 0, sizeof(BodySensor_Handle));
    pbs->huart = huart;
    return pbs;
}

BodySensor_MeasureData* BodySensor_Measuring(BodySensor_Handle *pbs) {
    
    // if(HAL_UART_Receive(pbs->huart, buffer, 10, 1000) != HAL_OK)
    //     return NULL;
    // PrintHEX(buffer, sizeof(buffer));
    return NULL;
}

void BodySensor_RxCpltCallback(BodySensor_Handle *pbs) {
    uint32_t startTime = SYS_TIME; // 接收开始时间
    BodySensor_SerialRxBuffer *rxBuffer = pmgr->rxBuffer;
    uint8_t c = rxBuffer->temp[0]; // 接收的字符
    if(pmgr->rxHeadStart && rxBuffer->index == 1 && c != 0xFF) {
        pmgr->rxHeadStart = FALSE;
        Messager_RxBufferClear(pmgr);
    }
    if(!pmgr->rxHeadStart && c == 0xAA) {
        pmgr->rxHeadStart = TRUE;
        Messager_RxBufferClear(pmgr);
    }
    if (pmgr->rxHeadStart && c == 0xCC && rxBuffer->size >= 30 && rxBuffer->data[rxBuffer->size - 2] == 0x55)
    {
        if (rxBuffer->receiveSize == 0 && rxBuffer->index + 1 >= 5) {
            rxBuffer->receiveSize = MergeToUint16(rxBuffer->data[3], rxBuffer->data[4]); // 从包头获取数据大小
        }
        if (rxBuffer->index + 1 < rxBuffer->receiveSize)
            rxBuffer->data[rxBuffer->index++] = c; // 追加字符
        else if (rxBuffer->index + 1 >= DATA_PACKET_MIN_SIZE)
        {
            rxBuffer->data[rxBuffer->index++] = c;
            uint8_t *tempBuffer = MESSAGE_GetTempBufferPoint(pmgr);
            memcpy(tempBuffer, rxBuffer->data, rxBuffer->size);
            pmgr->rxHeadStart = FALSE;
            Messager_RxBufferClear(pmgr);
            osMessagePut(pmgr->messageQueue, (uint32_t)tempBuffer, 100); // 推送数据包到消息队列
        }
    }
    else
        rxBuffer->data[rxBuffer->index++] = c;
    if (rxBuffer->index + 1 > rxBuffer->size)
        Messager_RxBufferClear(pmgr);
    // 开始接收下一个字符
    while (HAL_UART_Receive_IT(pmgr->huart, pmgr->rxBuffer->temp, 1) != HAL_OK)
    {
        // 判断接收是否超时
        if (SYS_TIME - startTime > pmgr->rxTimeout)
        {
            HAL_UART_AbortReceive_IT(pmgr->huart);  //超时终止接收
            Messager_RxBufferClear(pmgr);
            return;
        }
        osDelay(50);
    }
}

void BodySensor_FreeMeasureData(BodySensor_MeasureData *measureData) {
    vPortFree(measureData);
}


