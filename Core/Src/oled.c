#include "config.h"
#include "common.h"
#include "oled.h"
#include "i2c.h"
#include "u8g2.h"

#define OLED_COMMAND_ADDRESS 0x00
#define OLED_DATA_ADDRESS 0x40

void OLED_Open(OLED_Handle *const poled);
void OLED_Close(OLED_Handle *const poled);
void OLED_Clear(OLED_Handle *const poled);
uint8_t OLED_SendData(u8x8_t *u8x8, uint8_t msg, uint8_t argInt, void *argPtr);
uint8_t OLED_Delay(u8x8_t *u8x8, uint8_t msg, uint8_t argInt, void *argPtr);

OLED_Handle *OLED_Init(I2C_HandleTypeDef *hi2c)
{
    OLED_Handle *poled = pvPortMalloc(sizeof(OLED_Handle));
    memset(poled, 0, sizeof(OLED_Handle));
    poled->hi2c = hi2c;
    u8g2_t *u8g2 = pvPortMalloc(sizeof(u8g2_t));
    memset(u8g2, 0, sizeof(u8g2_t));
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, OLED_SendData, OLED_Delay); // 初始化u8g2
    poled->u8g2 = u8g2;
    u8g2_InitDisplay(poled->u8g2);     // 根据所选的芯片进行初始化工作，初始化完成后，显示器处于关闭状态
    OLED_Open(poled);
    OLED_Clear(poled);
    u8g2_SetFont(poled->u8g2, u8g2_font_6x13_tf);
    // u8g2_DrawUTF8(poled->u8g2, 0, 16, "OpenMOSS");
    // u8g2_SendBuffer(poled->u8g2);
    // u8g2_t u8g2;
    // u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_gpio_and_delay);
    // OLED_Close(poled);  //关闭显示器
    // OLED_SendCommand(poled, 0x20);  //设置内存寻址模式Set Memory Addressing Mode
    // OLED_SendCommand(poled, OLED_ADDRESSING_MODE);  // 00 水平寻址模式 01 垂直寻址模式 02 页面寻址模式(复位)
    // OLED_SendCommand(poled, 0x81);  //设置对比度
    // OLED_SendCommand(poled, OLED_CONTRAST);  //对比度，数值越大对比度越高
    // OLED_SendCommand(poled, 0xC8);  //扫描方向 不上下翻转Com scan direction
    // OLED_SendCommand(poled, 0xA1);  //设置段重新映射 不左右翻转set segment remap
    // OLED_SendCommand(poled, 0xA8);  //设置多路复用比(1-64)
    // OLED_SendCommand(poled, 0x3F);  //设定值1/32  1/32 duty
    // OLED_SendCommand(poled, 0xD3);  //设置显示偏移 set display offset
    // OLED_SendCommand(poled, 0x00);  //
    // OLED_SendCommand(poled, 0xD5);  //设置osc分区 set osc division
    // OLED_SendCommand(poled, 0x80);  //
    // OLED_SendCommand(poled, 0xD8);  //关闭区域颜色模式 set area color mode off
    // OLED_SendCommand(poled, 0x05);  //
    // OLED_SendCommand(poled, 0xD9);  //设置预充电期 Set Pre-Charge Period
    // OLED_SendCommand(poled, 0xF1);  //
    // OLED_SendCommand(poled, 0xDA);  //设置com引脚配置 set com pin configuartion
    // OLED_SendCommand(poled, 0x12);  //
    // OLED_SendCommand(poled, 0xDB);  //设置vcomh set Vcomh
    // OLED_SendCommand(poled, 0x30);  //
    // OLED_SendCommand(poled, 0x8D);  //设置电源泵启用 set charge pump enable
    // OLED_SendCommand(poled, 0x14);  //
    // OLED_SendCommand(poled, 0xA4);  //设置全局显示  bit0，1白，0黑
    // if(OLED_INVERSE_COLOR == TRUE)
    //     OLED_SendCommand(poled, 0xA7);  //反相显示
    // else
    //     OLED_SendCommand(poled, 0xA6);  //正常显示
    // OLED_Clear(poled, 0x00);  //清屏
    // OLED_Open(poled);  //开启显示器
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
    u8g2_SendBuffer(poled->u8g2);
}

void OLED_DrawString(OLED_Handle *const poled, uint8_t x, uint8_t y, uint8_t *const str)
{
    u8g2_DrawStr(poled->u8g2, x, y, (const char *)str);
    u8g2_SendBuffer(poled->u8g2);
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
        delayUs(5);
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
