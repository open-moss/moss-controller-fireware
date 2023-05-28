#include "stdlib.h"
#include "string.h"

#include "vl53l1_api.h"

#include "common.h"
#include "tof.h"

ToF_Handle* ToF_Init(I2C_HandleTypeDef* hi2c) {
    ToF_Handle *ptof = pvPortMalloc(sizeof(ToF_Handle));
    memset(ptof, 0, sizeof(ToF_Handle));
    VL53L1_Dev_t *pdevice = pvPortMalloc(sizeof(VL53L1_Dev_t));
    pdevice->I2cHandle = hi2c;
    pdevice->I2cDevAddr = 0x52;
    ptof->pdevice = pdevice;
    VL53L1_Error status = VL53L1_WaitDeviceBooted(ptof->pdevice) ||
    VL53L1_DataInit(ptof->pdevice) ||
    VL53L1_StaticInit(ptof->pdevice) ||
    VL53L1_SetDistanceMode(ptof->pdevice, VL53L1_DISTANCEMODE_LONG) ||
    VL53L1_SetMeasurementTimingBudgetMicroSeconds(ptof->pdevice, 50000) ||
    VL53L1_SetInterMeasurementPeriodMilliSeconds(ptof->pdevice, 500) ||
    VL53L1_StartMeasurement(ptof->pdevice);
    if(status != VL53L1_ERROR_NONE)
        return NULL;
    return ptof;
}

ToF_Info* ToF_GetDeviceInfo(ToF_Handle* const ptof) {
    ToF_Info *pinfo = pvPortMalloc(sizeof(ToF_Info));
    memset(pinfo, 0, sizeof(ToF_Info));
    VL53L1_Error status = VL53L1_RdByte(ptof->pdevice, 0x010F, &pinfo->modelId) || 
    VL53L1_RdByte(ptof->pdevice, 0x0110, &pinfo->moduleType) ||
    VL53L1_RdWord(ptof->pdevice, 0x010F, &pinfo->wordData);
    if(status != VL53L1_ERROR_NONE)
        return NULL;
    return pinfo;
}


