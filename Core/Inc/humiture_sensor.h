#ifndef HumitureSensor_H
#define HumitureSensor_H

#include "stm32f1xx_hal_i2c.h"

#include "common.h"

typedef struct {
    I2C_HandleTypeDef *hi2c;
} HumitureSensor_Handle;

typedef struct {
    float temperature;
    float humidity;
} HumitureSensor_MeasureData;

HumitureSensor_Handle *HumitureSensor_Init(I2C_HandleTypeDef *hi2c);
HumitureSensor_MeasureData* HumitureSensor_Measuring(HumitureSensor_Handle *phs);
HAL_StatusTypeDef HumitureSensor_Reset(HumitureSensor_Handle *phs);
void HumitureSensor_FreeMeasureData(HumitureSensor_MeasureData *measureData);

#endif
