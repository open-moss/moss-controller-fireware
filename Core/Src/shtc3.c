#include "string.h"

#include "common.h"
#include "shtc3.h"
#include "logger.h"

HAL_StatusTypeDef SHTC3_Send(I2C_HandleTypeDef* hi2c, uint16_t data);
HAL_StatusTypeDef SHTC3_Read(I2C_HandleTypeDef* hi2c, uint8_t* measureData);
HAL_StatusTypeDef SHTC3_Check(uint8_t msb, uint8_t lsb, uint8_t _crc);

HAL_StatusTypeDef SHTC3_Wakeup(I2C_HandleTypeDef* hi2c) {
    return SHTC3_Send(hi2c, SHTC3_COMMAND_WAKEUP);
}

SHTC3_MeasureData* SHTC3_Measuring(I2C_HandleTypeDef* hi2c) {
    if(SHTC3_Send(hi2c, SHTC3_COMMAND_MEASURING) != HAL_OK)
        return NULL;
    uint8_t buffer[6];
    memset(buffer, 0, sizeof(buffer));
    if(SHTC3_Read(hi2c, buffer) != HAL_OK)
        return NULL;
    if(SHTC3_Check(buffer[0], buffer[1], buffer[2]) != HAL_OK)
        return NULL;
    if(SHTC3_Check(buffer[3], buffer[4], buffer[5]) != HAL_OK)
        return NULL;
    SHTC3_MeasureData *measureData = pvPortMalloc(sizeof(SHTC3_MeasureData));
    memset(measureData, 0, sizeof(SHTC3_MeasureData));
    measureData->humidity =  100.0f * (float)MergeToUint16(buffer[0], buffer[1]) / 65535.0f;
    measureData->temperature = -45.0f + 175.0f * (float)MergeToUint16(buffer[3], buffer[4]) / 65535.0f;
    return measureData;
}

HAL_StatusTypeDef SHTC3_Check(uint8_t msb, uint8_t lsb, uint8_t _crc) {
    uint8_t data[2] = { msb, lsb };
    uint8_t crc = 0xFF;
	uint8_t poly = 0x31;
	for (uint8_t indi = 0; indi < 2; indi++) {
		crc ^= data[indi];
		for (uint8_t indj = 0; indj < 8; indj++) {
			if (crc & 0x80) {
				crc = (uint8_t)((crc << 1) ^ poly);
			} else {
				crc <<= 1;
			}
		}
	}
	if (_crc ^ crc)
        return HAL_ERROR;
    return HAL_OK;
}

HAL_StatusTypeDef SHTC3_Read(I2C_HandleTypeDef* hi2c, uint8_t* measureData) {
    return HAL_I2C_Master_Receive(hi2c, SHTC3_ADDRESS_READ, measureData, sizeof(uint8_t) * 6, 1000);
}

HAL_StatusTypeDef SHTC3_Send(I2C_HandleTypeDef* hi2c, uint16_t data) {
    uint8_t temp[2] = { ExtractUint8High(data), ExtractUint8Low(data) };
    HAL_StatusTypeDef state = HAL_I2C_Master_Transmit(hi2c, SHTC3_ADDRESS_WRITE, temp, sizeof(temp), 1000);
    osDelay(10);
    return state;
}
