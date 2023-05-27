#include "stdlib.h"
#include "string.h"

#include "vl53l1_api.h"

#include "common.h"
#include "tof.h"

ToF_Handle* TOF_Init(I2C_HandleTypeDef* hi2c) {
    ToF_Handle *ptof = pvPortMalloc(sizeof(ToF_Handle));
    memset(ptof, 0, sizeof(ToF_Handle));
    return ptof;
}


