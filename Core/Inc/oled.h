#ifndef OLED_H
#define OLED_H

#include <string.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"
#include "u8g2.h"

#define CmdReg 0x00  // 表示发送指令
#define DataReg 0x40 // 表示发送数据

#define OLED_BOOL_Replace ((uint8_t)0x00)  // 替换
#define OLED_BOOL_ADD ((uint8_t)0x01)      // 加
#define OLED_BOOL_Subtract ((uint8_t)0x02) // 减

typedef struct {
    I2C_HandleTypeDef *hi2c;
    u8g2_t *u8g2;
} OLED_Handle;

OLED_Handle *OLED_Init(I2C_HandleTypeDef *hi2c);
void OLED_Open(OLED_Handle *const poled);
void OLED_Close(OLED_Handle *const poled);
void OLED_DrawString(OLED_Handle *const holed, uint8_t x, uint8_t y, uint8_t *const str);

#endif
