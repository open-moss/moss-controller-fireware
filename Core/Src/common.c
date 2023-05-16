#include <stdio.h>
#include <stdarg.h>

#include "usart.h"
#include "common.h"
#include "config.h"

extern SemaphoreHandle_t printSemaphoreHandle;

int fputc(int c, FILE *f)
{
    uint8_t temp[1] = {c};
    HAL_UART_Transmit(&DEBUG_SERIAL_PORT_HUART, temp, 1, 100);
    return c;
}

void DebugPrintf(const char *format, const char name[17], uint32_t line, ...)
{
    xSemaphoreTake(printSemaphoreHandle, 100);
    va_list ap;
    va_start(ap, line);
    if (name != NULL && line != NULL)
        printf("[%s:%d]: ", name, line);
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
    xSemaphoreGive(printSemaphoreHandle);
}

void DebugPrintfISR(const char *format, const char name[17], uint32_t line, ...)
{
    xSemaphoreTakeFromISR(printSemaphoreHandle, pdFALSE);
    va_list ap;
    va_start(ap, line);
    if (name != NULL && line != NULL)
        printf("[%s:%d]: ", name, line);
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
    xSemaphoreGiveFromISR(printSemaphoreHandle, pdFALSE);
}

void DebugHEXPrint(uint8_t *buff, uint32_t size, const char name[17], uint32_t line)
{
    xSemaphoreTake(printSemaphoreHandle, 100);
    if (name != NULL && line != NULL)
        printf("[%s:%d]: ", name, line);
    for (uint32_t i = 0; i < size; i++)
    {
        printf("%02X", buff[i]);
        if (i < size - 1)
            printf(" ");
    }
    printf("\n");
    xSemaphoreGive(printSemaphoreHandle);
}

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
// void Delayus(uint16_t us)
// {
//     uint16_t differ = 0xffff - us - 5;
//     if (HAL_TIM_Base_Start_IT(delayTimer) != HAL_OK)
//     {
//         Debug_Printf("Delayus Failed", __FUNCTION__, __LINE__);
//         return;
//     }
//     __HAL_TIM_SetCounter(delayTimer, differ);
//     while (differ < 0xffff - 5)
//     {
//         differ = __HAL_TIM_GetCounter(delayTimer);
//     }
//     if (HAL_TIM_Base_Stop_IT(delayTimer) != HAL_OK)
//     {
//         Debug_Printf("Delayus Failed", __FUNCTION__, __LINE__);
//         return;
//     }
// }

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
