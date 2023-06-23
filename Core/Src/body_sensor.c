#include "string.h"

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include "body_sensor.h"
#include "logger.h"

BodySensor_Handle* BodySensor_Init(UART_HandleTypeDef* huart) {
    BodySensor_Handle *pbs = pvPortMalloc(sizeof(BodySensor_Handle));
    memset(pbs, 0, sizeof(BodySensor_Handle));
    pbs->huart = huart;
    return pbs;
}

BodySensor_MeasureData* BodySensor_Measuring(BodySensor_Handle *pbs) {
    static uint8_t buffer[30];
    // if(HAL_UART_Receive(pbs->huart, buffer, 1, 1000) != HAL_OK)
    //     return NULL;
    // PrintHEX(buffer, sizeof(buffer));
    return NULL;
}

void BodySensor_FreeMeasureData(BodySensor_MeasureData *measureData) {
    vPortFree(measureData);
}
