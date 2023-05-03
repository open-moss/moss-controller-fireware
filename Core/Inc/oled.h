#ifndef OLED_H
#define OLED_H

#include <string.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"

#define CmdReg 0x00     //表示发送指令
#define DataReg 0x40    //表示发送数据
// #define OLED_ON 0xaf    //打开显示器
// #define OLED_OFF 0xae   //关闭显示器

#define OLED_BOOL_Replace ((uint8_t)0x00)  //替换
#define OLED_BOOL_ADD ((uint8_t)0x01)      //加
#define OLED_BOOL_Subtract ((uint8_t)0x02) //减

typedef struct _OLED_Handle OLED_Handle;
typedef struct _OLED_Handle {
    I2C_HandleTypeDef* hi2c;
    uint8_t buffer[8][128];  //缓冲区
    uint8_t bufferFlag[128];  //刷新标志
}OLED_Handle;

OLED_Handle OLED_Init(I2C_HandleTypeDef* hi2c);
HAL_StatusTypeDef OLED_Refresh(OLED_Handle* const holed);
HAL_StatusTypeDef OLED_DrawString(OLED_Handle* const holed, uint8_t x, uint8_t y, uint8_t *str);

#endif
