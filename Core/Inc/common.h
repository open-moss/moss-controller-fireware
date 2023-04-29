#ifndef COMMON_H
#define COMMON_H
#include "stm32f1xx_hal.h"

#define FALSE 0
#define TRUE (!FALSE)
typedef unsigned char BOOL;

uint8_t getInt8Data(uint8_t *data, uint8_t start, uint8_t size);

#endif
