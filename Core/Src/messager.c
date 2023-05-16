#include "string.h"
#include "usart.h"

#include "common.h"
#include "config.h"
#include "messager.h"

MESSAGER_Handle MESSAGER_Init(UART_HandleTypeDef *huart) {
    MESSAGER_Handle hmessager;
    SerialRxBuffer rxBuffer;
    // memset(rxBuffer.data, 0, sizeof(rxBuffer.data));  //清空接收缓冲
    hmessager.huart = huart;
    hmessager.rxBuffer = &rxBuffer;
    return hmessager;
}



void MESSAGER_TxCpltCallback(MESSAGER_Handle* const hmessager) {
    hmessager->serialBusy = FALSE;
}

void MESSAGER_RxCpltCallback(MESSAGER_Handle* const hmessager) {
    uint32_t startTime = SYS_TIME;  //接收开始时间
    uint8_t c;  //接收的字符

}
