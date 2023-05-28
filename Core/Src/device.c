#include "stm32f1xx_hal.h"

#include "config.h"

HAL_StatusTypeDef Device_Power3V3Open() {
    return HAL_GPIO_WritePin(POWER3V3_GPIO_PORT, POWER3V3_PIN, GPIO_PIN_SET);
}

HAL_StatusTypeDef Device_Power3V3Close() {
    return HAL_GPIO_WritePin(POWER3V3_GPIO_PORT, POWER3V3_PIN, GPIO_PIN_RESET);
}

HAL_StatusTypeDef Device_Power5VOpen() {
    return HAL_GPIO_WritePin(POWER5V_GPIO_PORT, POWER5V_PIN, GPIO_PIN_SET);
}

HAL_StatusTypeDef Device_Power5VClose() {
    return HAL_GPIO_WritePin(POWER5V_GPIO_PORT, POWER5V_PIN, GPIO_PIN_RESET);
}

HAL_StatusTypeDef Device_LaserOpen() {
    return HAL_GPIO_WritePin(LASER_GPIO_PORT, LASER_PIN, GPIO_PIN_SET);
}

HAL_StatusTypeDef Device_LaserClose() {
    return HAL_GPIO_WritePin(LASER_GPIO_PORT, LASER_PIN, GPIO_PIN_RESET);
}

HAL_StatusTypeDef Device_DatalightOpen() {
    return HAL_GPIO_WritePin(DATALIGHT_GPIO_PORT, DATALIGHT_PIN, GPIO_PIN_SET);
}

HAL_StatusTypeDef Device_DatalightClose() {
    return HAL_GPIO_WritePin(DATALIGHT_GPIO_PORT, DATALIGHT_PIN, GPIO_PIN_RESET);
}

HAL_StatusTypeDef Device_StatusLEDSwitch() {
    
}

HAL_StatusTypeDef Device_StatusLEDClose() {
    return HAL_GPIO_WritePin(STATUS_RED_LED_GPIO_PORT, STATUS_RED_LED_PIN, GPIO_PIN_RESET) ||
    HAL_GPIO_WritePin(STATUS_BLUE_LED_GPIO_PORT, STATUS_BLUE_LED_PIN, GPIO_PIN_RESET);
}
