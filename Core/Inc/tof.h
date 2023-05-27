#ifndef TOF_H
#define TOF_H

#include "common.h"

typedef struct {
    VL53L1_Dev_t *pdevice;
} ToF_Handle;

ToF_Handle* ToF_Init(I2C_HandleTypeDef* hi2c);

#endif
