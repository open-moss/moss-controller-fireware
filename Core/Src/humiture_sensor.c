#include "string.h"

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include "humiture_sensor.h"
#include "shtc3.h"

HumitureSensor_Handle* HumitureSensor_Init(I2C_HandleTypeDef *hi2c) {
    HumitureSensor_Handle *phs = pvPortMalloc(sizeof(HumitureSensor_Handle));
    memset(phs, 0, sizeof(HumitureSensor_Handle));
    phs->hi2c = hi2c;
    HumitureSensor_Reset(phs);
    return phs;
}

HumitureSensor_MeasureData* HumitureSensor_Measuring(HumitureSensor_Handle *phs) {
    HumitureSensor_MeasureData *measureData = (HumitureSensor_MeasureData*)SHTC3_Measuring(phs->hi2c);
    return measureData;
}

HAL_StatusTypeDef HumitureSensor_Reset(HumitureSensor_Handle *phs) {
    return SHTC3_Wakeup(phs->hi2c);
}

void HumitureSensor_FreeMeasureData(HumitureSensor_MeasureData *measureData) {
    vPortFree(measureData);
}
