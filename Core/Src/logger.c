#include "stdio.h"
#include "stdarg.h"

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "usart.h"
#include "config.h"

extern SemaphoreHandle_t printSemaphoreHandle;

int fputc(int c, FILE *f)
{
    uint8_t temp[1] = {c};
    HAL_UART_Transmit(&DEBUG_SERIAL_PORT_HUART, temp, 1, 100);
    return c;
}

void __printf(const char *name, uint32_t line, uint8_t level, const char *format, ...)
{
    xSemaphoreTake(printSemaphoreHandle, 100);
    va_list ap;
    va_start(ap, line);
    if (name != NULL && line != NULL)
        printf("[%c][%s:%d]: ", level, name, line);
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
    xSemaphoreGive(printSemaphoreHandle);
}


void printfISR(const char *name, uint32_t line, uint8_t level, const char *format, ...)
{
    xSemaphoreTakeFromISR(printSemaphoreHandle, pdFALSE);
    va_list ap;
    va_start(ap, line);
    if (name != NULL && line != NULL)
        printf("[%c][%s:%d]: ", level, name, line);
    vprintf(format, ap);
    printf("\n");
    va_end(ap);
    xSemaphoreGiveFromISR(printSemaphoreHandle, pdFALSE);
}

void printHEX(const char *name, uint32_t line, uint8_t *buff, uint32_t size)
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

