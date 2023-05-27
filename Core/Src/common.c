#include "stdio.h"
#include "stdarg.h"

#include "usart.h"
#include "common.h"
#include "config.h"
#include "logger.h"

extern SemaphoreHandle_t printSemaphoreHandle;

/**
 * 打印系统任务列表
 */
void PrintTaskList()
{
    uint8_t pcWriteBuffer[300];
    vTaskList((char *)&pcWriteBuffer);
    printf("\n%s", pcWriteBuffer);
}

HAL_StatusTypeDef createTask(char *name, void (*task)(), osPriority priority, uint16_t stackSize)
{
    osThreadDef_t threadDef = {0};
    threadDef.name = name;
    threadDef.pthread = task;
    threadDef.tpriority = priority;
    threadDef.instances = 0;
    threadDef.stacksize = stackSize;
    osThreadId threadHandle = osThreadCreate(&threadDef, NULL);
    if(threadHandle == NULL)
        return HAL_ERROR;
    return HAL_OK;
}

/**
 * 时钟微秒级延迟
 * 
 * @param [in] us 微秒
 */
void delayUs(uint16_t us)
{
    uint16_t differ = 0xffff - us - 5;
    if (HAL_TIM_Base_Start_IT(DELAY_US_TIMER) != HAL_OK)
    {
        logError("delayUs Failed");
        return;
    }
    __HAL_TIM_SetCounter(DELAY_US_TIMER, differ);
    while (differ < 0xffff - 5)
    {
        differ = __HAL_TIM_GetCounter(DELAY_US_TIMER);
    }
    if (HAL_TIM_Base_Stop_IT(DELAY_US_TIMER) != HAL_OK)
    {
        logError("delayUs Failed");
        return;
    }
}

uint8_t extractUint8High(uint16_t data) {
    return (data >> 8) & 0xFF;
}

uint8_t extractUint8Low(uint16_t data) {
    return data & 0xFF;
}

uint16_t mergeToUint16(uint8_t high, uint8_t low) {
    return (high << 8) | low;
}

/**
 * 获取当前系统节拍数（开机到现在的毫秒数）
 */
uint32_t Get_System_Time(void)
{
    return xTaskGetTickCount();
}

/**
 * 获取当前系统节拍数（开机到现在的秒数）
 */
uint32_t Get_System_Time_Seconds(void)
{
    return xTaskGetTickCount() / 1000;
}
