#ifndef COMMON_H
#define COMMON_H
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

void DebugPrintf(const char *format, const char name[17], uint32_t line, ...);
void DebugPrintfISR(const char *format, const char name[17], uint32_t line, ...);
void DebugHEXPrint(uint8_t *buff, uint32_t size, const char name[17], uint32_t line);

#define FALSE 0
#define TRUE (!FALSE)
typedef unsigned char BOOL;

void PrintTaskList(void);
HAL_StatusTypeDef createTask(char *name, void (*task)(), osPriority priority, uint16_t stackSize);

#endif
