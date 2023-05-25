#ifndef TOF_H
#define TOF_H

#include "common.h"

typedef struct {
    I2C_HandleTypeDef *hi2c;
} ToF_Handle;

ToF_Handle* ToF_Init(I2C_HandleTypeDef* hi2c);

#endif
