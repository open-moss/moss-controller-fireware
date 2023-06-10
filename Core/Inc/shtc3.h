#ifndef SHTC3_H
#define SHTC3_H

#include "stm32f1xx_hal_i2c.h"

#define SHTC3_ADDRESS_WRITE 0xE0
#define SHTC3_ADDRESS_READ 0xE1
#define SHTC3_COMMAND_WAKEUP 0x3517
#define SHTC3_COMMAND_MEASURING 0x5C24

typedef struct {
    float temperature;
    float humidity;
} SHTC3_MeasureData;

HAL_StatusTypeDef SHTC3_Wakeup(I2C_HandleTypeDef* hi2c);
SHTC3_MeasureData* SHTC3_Measuring(I2C_HandleTypeDef* hi2c);

#endif
