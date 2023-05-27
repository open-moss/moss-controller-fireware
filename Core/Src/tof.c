#include "stdlib.h"
#include "string.h"

#include "vl53l1_api.h"

#include "common.h"
#include "tof.h"

ToF_Handle* TOF_Init(I2C_HandleTypeDef* hi2c) {
    ToF_Handle *ptof = pvPortMalloc(sizeof(ToF_Handle));
    memset(ptof, 0, sizeof(ToF_Handle));
    VL53L1_Dev_t *pdevice = pvPortMalloc(sizeof(VL53L1_Dev_t));
    pdevice->I2cHandle = hi2c;
    pdevice->I2cDevAddr = 0x52;
    ptof->pdevice = pdevice;
    
    return ptof;
}


