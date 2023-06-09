#ifndef HumitureSensor_H
#define HumitureSensor_H

#include "stm32f1xx_hal_i2c.h"

#include "common.h"

typedef struct {
    I2C_HandleTypeDef *hi2c;
} HumitureSensor_Handle;

HumitureSensor_Handle *HumitureSensor_Init(I2C_HandleTypeDef *hi2c);

#endif
