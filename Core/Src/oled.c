#include "config.h"
#include "oled.h"
#include "font_lib.h"

#define OLED_COMMAND_ADDRESS 0x00
#define OLED_DATA_ADDRESS 0x40

HAL_StatusTypeDef OLED_Open(OLED_Handle* const poled);
HAL_StatusTypeDef OLED_Close(OLED_Handle* const poled);
HAL_StatusTypeDef OLED_Clear(OLED_Handle* const poled, uint8_t data);
static HAL_StatusTypeDef OLED_SetPosition(OLED_Handle* const poled, uint8_t x, uint8_t page);
HAL_StatusTypeDef OLED_SendCommand(OLED_Handle* const poled, uint8_t data);
HAL_StatusTypeDef OLED_SendData(OLED_Handle* const poled, uint8_t data);
HAL_StatusTypeDef OLED_DrawBitmap(OLED_Handle* const poled, uint8_t x, uint8_t y, uint8_t width, uint8_t high, uint8_t *data);
static uint8_t getInt8Data(uint8_t *data, uint8_t start, uint8_t size);

OLED_Handle* OLED_Init(I2C_HandleTypeDef* hi2c) {
    OLED_Handle* poled = pvPortMalloc(sizeof(OLED_Handle));
    poled->hi2c = hi2c;
    poled->buffer = (uint8_t**)pvPortMalloc(sizeof(uint8_t*) * 8);
    for(uint8_t i = 0;i < 8;i++)
        poled->buffer[i] = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * 128);
    poled->bufferFlag = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * 128);
    OLED_Close(poled);  //关闭显示器
    OLED_SendCommand(poled, 0x20);  //设置内存寻址模式Set Memory Addressing Mode
    OLED_SendCommand(poled, OLED_ADDRESSING_MODE);  // 00 水平寻址模式 01 垂直寻址模式 02 页面寻址模式(复位)
    OLED_SendCommand(poled, 0x81);  //设置对比度
    OLED_SendCommand(poled, OLED_CONTRAST);  //对比度，数值越大对比度越高
    OLED_SendCommand(poled, 0xC8);  //扫描方向 不上下翻转Com scan direction
    OLED_SendCommand(poled, 0xA1);  //设置段重新映射 不左右翻转set segment remap
    OLED_SendCommand(poled, 0xA8);  //设置多路复用比(1-64)
    OLED_SendCommand(poled, 0x3F);  //设定值1/32  1/32 duty
    OLED_SendCommand(poled, 0xD3);  //设置显示偏移 set display offset
    OLED_SendCommand(poled, 0x00);  //
    OLED_SendCommand(poled, 0xD5);  //设置osc分区 set osc division
    OLED_SendCommand(poled, 0x80);  //
    OLED_SendCommand(poled, 0xD8);  //关闭区域颜色模式 set area color mode off
    OLED_SendCommand(poled, 0x05);  //
    OLED_SendCommand(poled, 0xD9);  //设置预充电期 Set Pre-Charge Period
    OLED_SendCommand(poled, 0xF1);  //
    OLED_SendCommand(poled, 0xDA);  //设置com引脚配置 set com pin configuartion
    OLED_SendCommand(poled, 0x12);  //
    OLED_SendCommand(poled, 0xDB);  //设置vcomh set Vcomh
    OLED_SendCommand(poled, 0x30);  //
    OLED_SendCommand(poled, 0x8D);  //设置电源泵启用 set charge pump enable
    OLED_SendCommand(poled, 0x14);  //
    OLED_SendCommand(poled, 0xA4);  //设置全局显示  bit0，1白，0黑
    if(OLED_INVERSE_COLOR == TRUE)
        OLED_SendCommand(poled, 0xA7);  //反相显示
    else
        OLED_SendCommand(poled, 0xA6);  //正常显示
    OLED_Clear(poled, 0x00);  //清屏
    OLED_Open(poled);  //开启显示器
    return poled;
}

/**
 * 开启OLED屏幕
 */
HAL_StatusTypeDef OLED_Open(OLED_Handle* const poled) {
    return OLED_SendCommand(poled, 0xAF);
}

/**
 * 关闭OLED屏幕
 */
HAL_StatusTypeDef OLED_Close(OLED_Handle* const poled) {
    return OLED_SendCommand(poled, 0xAE);
}

/**
 * 清空OLED屏幕
 */
HAL_StatusTypeDef OLED_Clear(OLED_Handle* const poled, uint8_t data) {
    uint16_t i;
    OLED_SetPosition(poled, 0, 0);
    for (i = 0; i < 8 * OLED_WIDTH; i++)
        OLED_SendData(poled, data);
    //更新缓存
    memset(poled->buffer, data, sizeof(poled->buffer));
    memset(poled->bufferFlag, 0, sizeof(poled->bufferFlag));
    return HAL_OK;
}

static HAL_StatusTypeDef OLED_SetVerticalPosition(OLED_Handle* const poled, uint8_t x, uint8_t up, uint8_t down) {
    OLED_SendCommand(poled, 0x22);  //设置页地址
    OLED_SendCommand(poled, up);    //起始
    OLED_SendCommand(poled, down);  //结束
    OLED_SendCommand(poled, 0x21);  //设置列地址
    OLED_SendCommand(poled, x);     //起始
    OLED_SendCommand(poled, 0x7F);  //结束
    return HAL_OK;
}

static HAL_StatusTypeDef OLED_SetPosition(OLED_Handle* const poled, uint8_t x, uint8_t page) {
    return OLED_SetVerticalPosition(poled, x, page, 0x07);
}

static HAL_StatusTypeDef OLED_AreaRefresh(OLED_Handle* const poled, uint8_t l, uint8_t r) {
    uint8_t count = 0, i, up = 0, down = 7, x, page;

    for (i = l; i <= r; i++) count |= poled->bufferFlag[i];

    while (!(count & (1 << up))) up++;

    while (!(count & (1 << down))) down--;

    OLED_SetVerticalPosition(poled, l, up, down);

    for (x = l; x <= r; x++)
        for (page = up; page <= down; page++)
            OLED_SendData(poled, poled->buffer[page][x]);

    memset(poled->bufferFlag + l, 0, r - l + 1);
    return HAL_OK;
}

HAL_StatusTypeDef OLED_Refresh(OLED_Handle* const poled) {
    uint8_t l = 0, r;

    while (l <= OLED_WIDTH) {
        while (l <= OLED_WIDTH && !poled->bufferFlag[l]) l++;

        r = l;
        while (r <= OLED_WIDTH && poled->bufferFlag[r]) r++;
        r--;

        if (l <= OLED_WIDTH) OLED_AreaRefresh(poled, l, r);

        l = r + 1;
    }
    return HAL_OK;
}

/**
 * 发送命令
 */
HAL_StatusTypeDef OLED_SendCommand(OLED_Handle* const poled, uint8_t data) {
    return HAL_I2C_Mem_Write(poled->hi2c, OLED_I2C_ADDRESS, OLED_COMMAND_ADDRESS, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xff);
}

/**
 * 发送数据
 */
HAL_StatusTypeDef OLED_SendData(OLED_Handle* const poled, uint8_t data) {
    return HAL_I2C_Mem_Write(poled->hi2c, OLED_I2C_ADDRESS, OLED_DATA_ADDRESS, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xff);
}

HAL_StatusTypeDef OLED_DrawColumn(OLED_Handle* const poled, uint8_t x, uint8_t y, uint8_t *data, uint8_t size, uint8_t bool) {
    if (x >= OLED_WIDTH || y + size > OLED_HEIGHT) return HAL_ERROR;
    uint8_t pos = 0;
    while (size > 0) {
        uint8_t data_n = poled->buffer[y / 8][x], delta;

        if (bool == OLED_BOOL_Replace)
            if (8 - (y % 8) >= size) {
                data_n = data_n - (data_n & ((0xff & (0xff << (8 - size))) >>
                                             (8 - size - (y % 8))));
                data_n |= getInt8Data(data, pos, size) << (y % 8);
                delta = size;
            } else {
                data_n = data_n - (data_n & (0xff << (y % 8)));
                data_n |= getInt8Data(data, pos, 8 - (y % 8)) << (y % 8);
                delta = 8 - (y % 8);
            }
        else if (bool == OLED_BOOL_ADD)
            if (8 - (y % 8) >= size) {
                data_n |= getInt8Data(data, pos, size) << (y % 8);
                delta = size;
            } else {
                data_n |= getInt8Data(data, pos, 8 - (y % 8)) << (y % 8);
                delta = 8 - (y % 8);
            }
        else if (bool == OLED_BOOL_Subtract)
            if (8 - (y % 8) >= size) {
                data_n &= ~(getInt8Data(data, pos, size) << (y % 8));
                delta = size;
            } else {
                data_n &= ~(getInt8Data(data, pos, 8 - (y % 8)) << (y % 8));
                delta = 8 - (y % 8);
            }

        if (data_n != poled->buffer[y / 8][x])
            poled->bufferFlag[x] |= 1 << (y / 8);
        poled->buffer[y / 8][x] = data_n;

        size -= delta;
        y += delta;
        pos += delta;
    }

    return HAL_OK;
}

HAL_StatusTypeDef OLED_DrawChar(OLED_Handle* const poled, uint8_t x, uint8_t y, uint8_t c) {
    return OLED_DrawBitmap(poled, x, y, 6, 8, ((uint8_t *)F6X8) + 6 * (c - 32));
}

HAL_StatusTypeDef OLED_DrawString(OLED_Handle* const poled, uint8_t x, uint8_t y, uint8_t *str) {
    uint16_t i = 0;

    uint8_t ret = 0;

    while (str[i] != '\0') {
        if (x + 5 >= OLED_WIDTH) {
            x = 0;
            y += 8;
        }

        ret |= OLED_DrawChar(poled, x, y, str[i]);

        x += 6;
        i++;
    }

    return ret ? HAL_ERROR : HAL_OK;
}

HAL_StatusTypeDef OLED_DrawBitmap(OLED_Handle* const poled, uint8_t x, uint8_t y, uint8_t width, uint8_t high, uint8_t *data) {
    uint8_t ret = 0;
    while (width > 0) {
        ret |= OLED_DrawColumn(poled, x, y, data, high, OLED_BOOL_Replace);
        data += (high + 7) / 8;
        width--;
        x++;
    }

    return ret ? HAL_ERROR : HAL_OK;
}

static uint8_t getInt8Data(uint8_t *data, uint8_t start, uint8_t size) {
    uint8_t ret = 0, delta;

    ret = data[start / 8] >> (start % 8);
    delta = 8 - (start % 8);
    size -= delta;
    start += delta;

    if (((int8_t)size) <= 0)
        return ret & (0xFF >> (-((int8_t)size)));
    else
        return ret | (getInt8Data(data, start, size) << delta);
}
