#ifndef COMMON_H
#define COMMON_H
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

void DelayUs(uint16_t us);
uint32_t GetSystemTime(void);
uint32_t GetSystemSecondsTime(void);
uint8_t ExtractUint8High(uint16_t data);
uint8_t ExtractUint8Low(uint16_t data);
uint16_t MergeToUint16(uint8_t high, uint8_t low);
void Uint16ToUint8Array(uint16_t data, uint8_t* pdata);

#define FALSE 0
#define TRUE (!FALSE)
typedef unsigned char BOOL;

void PrintTaskList(void);
HAL_StatusTypeDef CreateTask(char *name, void (*task)(), osPriority priority, uint16_t stackSize);

#define SYS_TIME GetSystemTime()
#define SYS_TIME_SEC GetSystemSecondsTime()

#endif
