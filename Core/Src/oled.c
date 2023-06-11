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

OLED_Handle *OLED_Init(I2C_HandleTypeDef *hi2c, osMessageQId *messageQueue, uint16_t bufferLength, uint8_t bufferCount, uint16_t messageBufferLength, uint8_t messageBufferCount)
{
    OLED_Handle *poled = pvPortMalloc(sizeof(OLED_Handle));
    memset(poled, 0, sizeof(OLED_Handle));
    poled->scene = OLED_SCENE_NORMAL;
    poled->hi2c = hi2c;
    poled->bufferIndex = 0;
    poled->bufferCount = bufferCount;
    poled->bufferLength = bufferLength;
    poled->buffers = (uint8_t**)pvPortMalloc(sizeof(uint8_t*) * bufferCount);
    for(uint8_t i = 0;i < bufferCount;i++) {
        poled->buffers[i] = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * bufferLength);
        memset(poled->buffers[i], 0, sizeof(uint8_t) * bufferLength);
    }
    u8g2_t *u8g2 = pvPortMalloc(sizeof(u8g2_t));
    memset(u8g2, 0, sizeof(u8g2_t));
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, OLED_SendData, OLED_Delay); // 初始化u8g2
    poled->u8g2 = u8g2;
    poled->messageQueue = messageQueue;
    poled->messageBufferIndex = 0;
    poled->messageBufferLength = messageBufferLength;
    poled->messageBufferCount = messageBufferCount;
    poled->messageBuffer = (uint8_t**)pvPortMalloc(sizeof(uint8_t*) * messageBufferCount);
    for(uint8_t i = 0;i < messageBufferCount;i++) {
        poled->messageBuffer[i] = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * messageBufferLength);
        memset(poled->messageBuffer[i], 0, sizeof(uint8_t) * messageBufferLength);
    }
    u8g2_InitDisplay(poled->u8g2);     // 根据所选的芯片进行初始化工作，初始化完成后，显示器处于关闭状态
    OLED_Open(poled);
    OLED_Clear(poled);
    OLED_Refresh(poled);
    u8g2_SetFont(poled->u8g2, u8g2_font_6x12_tf);
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
    memcpy(buffer, str, poled->messageBufferLength);
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
    uint8_t *messageBuffer = event.value.p;
    uint16_t messageBufferLength = strlen((const char*)messageBuffer) * sizeof(uint8_t);
    uint8_t chunkSize = ceil((float)messageBufferLength / poled->bufferLength);
    if(chunkSize == 0) chunkSize = 1;
    uint8_t offset = 0;
    while(offset < chunkSize * poled->bufferLength) {
        if(poled->bufferIndex > poled->bufferCount - 1) {
            for(uint8_t i = 0;i < poled->bufferCount - 1;i++) {
                memset(poled->buffers[i], 0, sizeof(uint8_t) * poled->bufferLength);
                memcpy(poled->buffers[i], poled->buffers[i + 1], sizeof(uint8_t) * poled->bufferLength);
            }
            poled->bufferIndex = poled->bufferCount - 1;
        }
        OLED_PartClear(poled, 0, 0, 128, 12);
        for(uint8_t i = 0;i < poled->bufferIndex;i++) {
            OLED_PartClear(poled, 0, i * 12 + 12, 128, 12);
            OLED_DrawString(poled, 0, i * 12 + 12, poled->buffers[i]);
        }
        memset(poled->buffers[poled->bufferIndex], 0, sizeof(uint8_t) * poled->bufferLength);
        memcpy(poled->buffers[poled->bufferIndex], messageBuffer + offset, sizeof(uint8_t) * poled->bufferLength);
        uint8_t temp[poled->bufferLength];
        for(uint8_t i = 0;i < strlen((const char*)poled->buffers[poled->bufferIndex]);i++) {
            memset(temp, 0, poled->bufferLength);
            memcpy(temp, poled->buffers[poled->bufferIndex], i + 1);
            OLED_PartClear(poled, 0, poled->bufferIndex * 12 + 12, 128, 12);
            OLED_DrawString(poled, 0, poled->bufferIndex * 12 + 12, temp);
            OLED_Refresh(poled);
            osDelay(10);
        }
        offset += poled->bufferLength;
        poled->bufferIndex++;
    }
    
}

uint8_t* OLED_GetMessageBufferPoint(OLED_Handle *const poled) {
    if(poled->messageBufferIndex >= poled->messageBufferCount)
        poled->messageBufferIndex = 0;
    memset(poled->messageBuffer[poled->messageBufferIndex], 0, sizeof(uint8_t) * poled->messageBufferLength);
    return poled->messageBuffer[poled->messageBufferIndex++];
}

uint8_t OLED_SendData(u8x8_t *u8x8, uint8_t msg, uint8_t argInt, void *argPtr)
{
    static uint8_t buffer[128];
    static uint8_t bufferIndex;
    uint8_t *data;

    switch (msg)
    {

    case U8X8_MSG_BYTE_START_TRANSFER:
    {
        bufferIndex = 0;
    }
    break;

    case U8X8_MSG_BYTE_SEND:
    {
        data = (uint8_t *)argPtr;

        while (argInt > 0)
        {
            buffer[bufferIndex++] = *data;
            data++;
            argInt--;
        }
    }
    break;

    case U8X8_MSG_BYTE_END_TRANSFER:
    {
        if (HAL_I2C_Master_Transmit(&OLED_HI2C, OLED_I2C_ADDRESS, buffer, bufferIndex, 1000) != HAL_OK)
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
