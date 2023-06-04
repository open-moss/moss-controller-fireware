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
    OLED_SCENE_NORMAL = 0,  //常规
    OLED_SCENE_SCROLL_SCREEN = 1  //滚动屏幕
} OLED_SceneType;

typedef struct {
    uint8_t x;
    uint8_t y;
} OLED_SceneContext;

typedef struct {
    OLED_SceneType scene;
    OLED_SceneContext *sceneCtx;
    I2C_HandleTypeDef *hi2c;
    osMessageQId *messageQueue;
    u8g2_t *u8g2;
    uint8_t bufferIndex;
    uint8_t **buffers;
    uint16_t bufferSize;
    uint8_t bufferCount;
} OLED_Handle;

OLED_Handle *OLED_Init(I2C_HandleTypeDef *hi2c, osMessageQId *messageQueue, uint16_t bufferSize, uint8_t bufferCount);
void OLED_Open(OLED_Handle *const poled);
void OLED_Close(OLED_Handle *const poled);
void OLED_Clear(OLED_Handle *const poled);
void OLED_Refresh(OLED_Handle *const poled);
void OLED_PartClear(OLED_Handle *const poled, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void OLED_DrawString(OLED_Handle *const holed, uint8_t x, uint8_t y, uint8_t *const str);
void OLED_PushString(OLED_Handle *const poled, uint8_t *const str);
void OLED_MessageHandle(OLED_Handle *const poled);

#endif
