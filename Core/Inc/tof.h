#ifndef TOF_H
#define TOF_H
#include "vl53l1_api.h"
#include "common.h"

typedef struct {
    VL53L1_Dev_t *pdevice;
} ToF_Handle;

typedef struct {
    uint8_t modelId;
    uint8_t moduleType;
    uint16_t wordData;
} ToF_Info;

ToF_Handle* ToF_Init(I2C_HandleTypeDef* hi2c);
ToF_Info* ToF_GetDeviceInfo(ToF_Handle* const ptof);
int16_t ToF_GetRangeMilliMeter(ToF_Handle* const ptof);

#endif
