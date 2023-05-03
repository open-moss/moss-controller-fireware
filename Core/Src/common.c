#include <stdio.h>
#include <stdarg.h>

#include "usart.h"
#include "common.h"

extern SemaphoreHandle_t printSemaphoreHandle;

int fputc(int c, FILE *f)
{
    uint8_t temp[1] = {c};
    HAL_UART_Transmit(&huart2, temp, 1, 100);
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
