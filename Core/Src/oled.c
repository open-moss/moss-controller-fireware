#include "math.h"

#include "config.h"
#include "common.h"
#include "oled.h"
#include "i2c.h"
#include "u8g2.h"
#include "logger.h"

#define OLED_COMMAND_ADDRESS 0x00
#define OLED_DATA_ADDRESS 0x40

extern SemaphoreHandle_t testSemaphoreHandle;

void OLED_Open(OLED_Handle *const poled);
void OLED_Close(OLED_Handle *const poled);
void OLED_Clear(OLED_Handle *const poled);
uint8_t OLED_SendData(u8x8_t *u8x8, uint8_t msg, uint8_t argInt, void *argPtr);
uint8_t OLED_Delay(u8x8_t *u8x8, uint8_t msg, uint8_t argInt, void *argPtr);
uint8_t* OLED_GetMessageBufferPoint(OLED_Handle *const poled);

OLED_Handle *OLED_Init(I2C_HandleTypeDef *hi2c, osMessageQId *messageQueue, const uint8_t *font, uint16_t textColumns, uint8_t textLines, uint16_t textBufferLength, uint8_t textBufferCount) {
    OLED_Handle *poled = pvPortMalloc(sizeof(OLED_Handle));
    memset(poled, 0, sizeof(OLED_Handle));
    poled->scene = OLED_SCENE_NORMAL;
    poled->hi2c = hi2c;
    poled->textIndex = 0;
    poled->textColumns = textColumns;
    poled->textLines = textLines;
    poled->texts = (uint8_t**)pvPortMalloc(sizeof(uint8_t*) * textLines);
    for(uint8_t i = 0;i < textLines;i++) {
        poled->texts[i] = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * textColumns);
        memset(poled->texts[i], 0, sizeof(uint8_t) * textColumns);
    }
    u8g2_t *u8g2 = pvPortMalloc(sizeof(u8g2_t));
    memset(u8g2, 0, sizeof(u8g2_t));
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, OLED_SendData, OLED_Delay); // 初始化u8g2
    poled->u8g2 = u8g2;
    poled->messageQueue = messageQueue;
    poled->textBufferIndex = 0;
    poled->textBufferLength = textBufferLength;
    poled->textBufferCount = textBufferCount;
    poled->textBuffers = (uint8_t**)pvPortMalloc(sizeof(uint8_t*) * textBufferCount);
    for(uint8_t i = 0;i < textBufferCount;i++) {
        poled->textBuffers[i] = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * textBufferLength);
        memset(poled->textBuffers[i], 0, sizeof(uint8_t) * textBufferLength);
    }
    u8g2_InitDisplay(poled->u8g2);
    OLED_Open(poled);
    OLED_Clear(poled);
    OLED_Refresh(poled);
    u8g2_SetFont(poled->u8g2, font);
    return poled;
}
  
void OLED_Open(OLED_Handle *const poled)
{
    u8g2_SetPowerSave(poled->u8g2, 0);
}

void OLED_Close(OLED_Handle *const poled)
{
    u8g2_SetPowerSave(poled->u8g2, 1);
}

/**
 * 清空OLED屏幕
 */
void OLED_Clear(OLED_Handle *const poled)
{
    u8g2_ClearBuffer(poled->u8g2);
}

void OLED_PartClear(OLED_Handle *const poled, uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    u8g2_SetDrawColor(poled->u8g2, 0);
    u8g2_DrawBox(poled->u8g2, x, y, width, height);
}

void OLED_DrawString(OLED_Handle *const poled, uint8_t x, uint8_t y, uint8_t *const str)
{
    u8g2_SetDrawColor(poled->u8g2, 1);
    u8g2_DrawStr(poled->u8g2, x, y, (const char *)str);
}

void OLED_PushString(OLED_Handle *const poled, uint8_t *const str) {
    uint8_t *buffer = OLED_GetMessageBufferPoint(poled);
    memcpy(buffer, str, poled->textBufferLength);
    osMessagePut(poled->messageQueue, (uint32_t)buffer, 100);
}

void OLED_Refresh(OLED_Handle *const poled) {
    u8g2_SendBuffer(poled->u8g2);
}

void OLED_Free(OLED_Handle *const poled) {
    
}

void OLED_MessageHandle(OLED_Handle *const poled) {
    osEvent event = osMessageGet(poled->messageQueue, 1000); //消息队列接收消息
    if (event.status != osEventMessage)
        return;
    uint8_t *textBuffers = event.value.p;
    uint16_t textBufferLength = strlen((const char*)textBuffers) * sizeof(uint8_t);
    uint8_t chunkSize = ceil((float)textBufferLength / poled->textColumns);
    if(chunkSize == 0) chunkSize = 1;
    uint8_t offset = 0;
    while(offset < chunkSize * poled->textColumns) {
        if(poled->textIndex > poled->textLines - 1) {
            for(uint8_t i = 0;i < poled->textLines - 1;i++) {
                memset(poled->texts[i], 0, sizeof(uint8_t) * poled->textColumns);
                memcpy(poled->texts[i], poled->texts[i + 1], sizeof(uint8_t) * poled->textColumns);
            }
            poled->textIndex = poled->textLines - 1;
        }
        OLED_PartClear(poled, 0, 0, 128, 12);
        for(uint8_t i = 0;i < poled->textIndex;i++) {
            OLED_PartClear(poled, 0, i * 12 + 12, 128, 12);
            OLED_DrawString(poled, 0, i * 12 + 12, poled->texts[i]);
        }
        memset(poled->texts[poled->textIndex], 0, sizeof(uint8_t) * poled->textColumns);
        memcpy(poled->texts[poled->textIndex], textBuffers + offset, sizeof(uint8_t) * poled->textColumns);
        uint8_t temp[poled->textColumns];
        for(uint8_t i = 0;i < strlen((const char*)poled->texts[poled->textIndex]);i++) {
            memset(temp, 0, poled->textColumns);
            memcpy(temp, poled->texts[poled->textIndex], i + 1);
            OLED_PartClear(poled, 0, poled->textIndex * 12 + 12, 128, 12);
            OLED_DrawString(poled, 0, poled->textIndex * 12 + 12, temp);
            OLED_Refresh(poled);
            osDelay(10);
        }
        offset += poled->textColumns;
        poled->textIndex++;
    }
    
}

uint8_t* OLED_GetMessageBufferPoint(OLED_Handle *const poled) {
    if(poled->textBufferIndex >= poled->textBufferCount)
        poled->textBufferIndex = 0;
    memset(poled->textBuffers[poled->textBufferIndex], 0, sizeof(uint8_t) * poled->textBufferLength);
    return poled->textBuffers[poled->textBufferIndex++];
}

uint8_t OLED_SendData(u8x8_t *u8x8, uint8_t msg, uint8_t argInt, void *argPtr)
{
    static uint8_t buffer[128];
    static uint8_t textIndex;
    uint8_t *data;

    switch (msg)
    {

    case U8X8_MSG_BYTE_START_TRANSFER:
    {
        textIndex = 0;
    }
    break;

    case U8X8_MSG_BYTE_SEND:
    {
        data = (uint8_t *)argPtr;

        while (argInt > 0)
        {
            buffer[textIndex++] = *data;
            data++;
            argInt--;
        }
    }
    break;

    case U8X8_MSG_BYTE_END_TRANSFER:
    {
        if (HAL_I2C_Master_Transmit(&OLED_HI2C, OLED_I2C_ADDRESS, buffer, textIndex, 1000) != HAL_OK)
            return 0;
    }
    break;

    case U8X8_MSG_BYTE_SET_DC:
        break;

    default:
        return 0;
    }

    return 1;
}

uint8_t OLED_Delay(u8x8_t *u8x8, uint8_t msg, uint8_t argInt, void *argPtr)
{
    switch (msg)
    {
    case U8X8_MSG_DELAY_100NANO: // delay argInt * 100 nano seconds
        __NOP();
        break;
    case U8X8_MSG_DELAY_10MICRO: // delay argInt * 10 micro seconds
        for (uint16_t n = 0; n < 320; n++)
        {
            __NOP();
        }
        break;
    case U8X8_MSG_DELAY_MILLI: // delay argInt * 1 milli second
        osDelay(1);
        break;
    case U8X8_MSG_DELAY_I2C: // argInt is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        DelayUs(5);
        break;                    // argInt=1: delay by 5us, argInt = 4: delay by 1.25us
    case U8X8_MSG_GPIO_I2C_CLOCK: // argInt=0: Output low at I2C clock pin
        break;                    // argInt=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:  // argInt=0: Output low at I2C data pin
        break;                    // argInt=1: Input dir with pullup high for I2C data pin
    case U8X8_MSG_GPIO_MENU_SELECT:
        u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_NEXT:
        u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_PREV:
        u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_HOME:
        u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
        break;
    default:
        u8x8_SetGPIOResult(u8x8, 1); // default return value
        break;
    }
    return 1;
}
