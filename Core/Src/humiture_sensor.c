#include "stm32f1xx_hal.h"

#include "humiture_sensor.h"

HumitureSensor_Handle *HumitureSensor_Init(I2C_HandleTypeDef *hi2c) {
    HumitureSensor_Handle *phs = pvPortMalloc(sizeof(HumitureSensor_Handle));
    memset(phs, 0, sizeof(HumitureSensor_Handle));
    phs->hi2c = hi2c;
    return phs;
}


