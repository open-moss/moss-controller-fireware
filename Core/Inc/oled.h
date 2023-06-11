#ifndef OLED_H
#define OLED_H

#include "string.h"
#include "stm32f1xx_hal_i2c.h"

#include "u8g2.h"

#include "common.h"

#define CmdReg 0x00  // 表示发送指令
#define DataReg 0x40 // 表示发送数据

#define OLED_BOOL_Replace ((uint8_t)0x00)  // 替换
#define OLED_BOOL_ADD ((uint8_t)0x01)      // 加
#define OLED_BOOL_Subtract ((uint8_t)0x02) // 减

typedef enum {
    OLED_SCENE_DEFAULT = 0,  //默认
    OLED_SCENE_SCROLL_SCREEN = 1  //滚动屏幕
} OLED_SceneType;

typedef struct {
    OLED_SceneType scene;
    I2C_HandleTypeDef *hi2c;
    osMessageQId *messageQueue;
    uint8_t **textBuffers;
    uint8_t textBufferIndex;
    uint16_t textBufferLength;
    uint8_t textBufferCount;
    u8g2_t *u8g2;
    uint8_t **texts;
    uint16_t textColumns;
    uint8_t textLines;
    uint8_t textIndex;
} OLED_Handle;

OLED_Handle *OLED_Init(I2C_HandleTypeDef *hi2c, osMessageQId *messageQueue, const uint8_t *font, uint16_t textColumns, uint8_t textLines, uint16_t messageBufferLength, uint8_t messageBufferCount);
void OLED_Open(OLED_Handle *const poled);
void OLED_Close(OLED_Handle *const poled);
void OLED_Clear(OLED_Handle *const poled);
void OLED_Refresh(OLED_Handle *const poled);
void OLED_PartClear(OLED_Handle *const poled, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void OLED_DrawString(OLED_Handle *const holed, uint8_t x, uint8_t y, uint8_t *const str);
void OLED_PushString(OLED_Handle *const poled, uint8_t *const str);
void OLED_MessageHandle(OLED_Handle *const poled);
BOOL OLED_MessageQueueIsFull(OLED_Handle *const poled);

#endif
