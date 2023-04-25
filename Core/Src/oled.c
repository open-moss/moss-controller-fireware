#include <stdlib.h>

#include "config.h"
#include "oled.h"

OLED_Handle* OLED_Init(I2C_HandleTypeDef hi2c) {
    OLED_Handle* holed = (OLED_Handle*)malloc(sizeof(OLED_Handle));

    return holed;
}

// void init(OLED* const pDriver);
// void on(OLED* const pDriver);
// void off(OLED* const pDriver);
// void clear(OLED* const pDriver, uint8_t data);
// void areaRefresh(OLED* const pDriver, uint8_t l, uint8_t r);
// void refresh(OLED* const pDriver);
// HAL_StatusTypeDef drawChar(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t c, uint8_t bool);
// HAL_StatusTypeDef drawString(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t *str, uint8_t bool);
// HAL_StatusTypeDef replaceString(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t *str);
// HAL_StatusTypeDef drawBitmap(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t *data, uint8_t bool);

// OLED* new_OLED(I2C_HandleTypeDef hi2c) {
//     OLED* pDriver = NULL;
//     pDriver = (OLED*)malloc(sizeof(OLED));
//     if(pDriver == NULL)
//         return NULL;
//     pDriver->hi2c = hi2c;
//     pDriver->init = init;
//     pDriver->on = on;
//     pDriver->off = off;
//     pDriver->clear = clear;
//     pDriver->refresh = refresh;
//     pDriver->drawChar = drawChar;
//     pDriver->drawString = drawString;
//     pDriver->replaceString = replaceString;
//     pDriver->drawBitmap = drawBitmap;
//     pDriver->delete = delete_OLED;
//     return pDriver;
// }

// void delete_OLED(OLED* const pDriver) {
//     free(pDriver);
// }

// /**
//  * @description: 发送语句至OLED
//  * @param {uint8_t} arg DataReg 发送数据
//  *                      CmdReg  发送指令
//  * @param {uint8_t} data    1字节数据
//  * @return {HAL_StatusTypeDef} HAL 状态
//  */
// HAL_StatusTypeDef OLED_sendCommand(OLED* const pDriver, uint8_t data) {
//     return HAL_I2C_Mem_Write(&pDriver->hi2c, OLED_I2C_ADDRESS, OLED_COMMAND_ADDRESS, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
// }

// HAL_StatusTypeDef OLED_sendData(OLED* const pDriver, uint8_t data) {
//     return HAL_I2C_Mem_Write(&pDriver->hi2c, OLED_I2C_ADDRESS, OLED_DATA_ADDRESS, I2C_MEMADD_SIZE_8BIT,  &data, 1, 100);
// }

// /**
//  * @description:  以硬件I2C方式初始化OLED
//  * @param {I2C_HandleTypeDef} hi2c 硬件i2c句柄
//  * @return {*}
//  */
// void OLED_init(OLED* const pDriver) {
//     off(pDriver);  //关闭屏幕
//     OLED_sendCommand(pDriver, 0x20);  //设置内存寻址模式Set Memory Addressing Mode
//     OLED_sendCommand(pDriver, OLED_ADDRESSING_MODE);  //00 水平寻址模式 | 01 垂直寻址模式 | 02 页面寻址模式(复位)
//     OLED_sendCommand(pDriver, 0x81);  //设置对比度
//     OLED_sendCommand(pDriver, OLED_CONTRAST);  //对比度,数值越大对比度越高
//     OLED_sendCommand(pDriver, 0xC8);  //扫描方向 不上下翻转Com scan direction
//     OLED_sendCommand(pDriver, 0xA1);  //设置段重新映射 不左右翻转set segment remap
//     OLED_sendCommand(pDriver, 0xA8);  //设置多路复用比(1-64)
//     OLED_sendCommand(pDriver, 0x3F);  //设定值1/32  1/32 duty
//     OLED_sendCommand(pDriver, 0xD3);  //设置显示偏移 set display offset
//     OLED_sendCommand(pDriver, 0x00);  //
//     OLED_sendCommand(pDriver, 0xD5);  //设置osc分区 set osc division
//     OLED_sendCommand(pDriver, 0x80);  //
//     OLED_sendCommand(pDriver, 0xD8);  //关闭区域颜色模式 set area color mode off
//     OLED_sendCommand(pDriver, 0x05);  //
//     OLED_sendCommand(pDriver, 0xD9);  //设置预充电期 Set Pre-Charge Period
//     OLED_sendCommand(pDriver, 0xF1);  //
//     OLED_sendCommand(pDriver, 0xDA);  //设置com引脚配置 set com pin configuartion
//     OLED_sendCommand(pDriver, 0x12);  //
//     OLED_sendCommand(pDriver, 0xDB);  //设置vcomh set Vcomh
//     OLED_sendCommand(pDriver, 0x30);  //
//     OLED_sendCommand(pDriver, 0x8D);  //设置电源泵启用 set charge pump enable
//     OLED_sendCommand(pDriver, 0x14);  //
//     OLED_sendCommand(pDriver, 0xA4);  //设置全局显示  bit0，1白，0黑
//     if(OLED_INVERSE_COLOR == TRUE)
//         OLED_sendCommand(pDriver, 0xA7);  //反相显示
//     else
//         OLED_sendCommand(pDriver, 0xA6);  //正常显示
//     clear(pDriver, 0x00);                      //清屏
//     on(pDriver);  //开启屏幕
// }

// void on(OLED* const pDriver) {
//     OLED_sendCommand(pDriver, 0xAF);  //打开显示器
// }

// void off(OLED* const pDriver) {
//     OLED_sendCommand(pDriver, 0xAE);  //关闭显示器
// }

// /**
//  * @description: 设置垂直寻址模式下坐标
//  * @param {uint8_t} x 起始列地址
//  * @param {uint8_t} up 起始页地址
//  * @param {uint8_t} down   结束页地址
//  * @return {*}
//  */
// static void setVerticalPosition(OLED* const pDriver, uint8_t x, uint8_t up, uint8_t down) {
//     OLED_sendCommand(pDriver, 0x22);  //设置页地址
//     OLED_sendCommand(pDriver, up);    //起始
//     OLED_sendCommand(pDriver, down);  //结束
//     OLED_sendCommand(pDriver, 0x21);  //设置列地址
//     OLED_sendCommand(pDriver, x);     //起始
//     OLED_sendCommand(pDriver, 0x7F);  //结束
// }

// /**
//  * @description: 设置垂直寻址模式下坐标，保持结束页地址为结尾
//  * @param {uint8_t} x 起始列地址
//  * @param {uint8_t} page 起始页地址
//  * @return {*}
//  */
// void setPosition(OLED* const pDriver, uint8_t x, uint8_t page) {
//     setVerticalPosition(pDriver, x, page, 0x07);
// }

// /**
//  * @description: 清空缓存，并更新屏幕
//  * @param {*}
//  * @return {*}
//  */
// void clear(OLED* const pDriver, uint8_t data) {
//     uint16_t i;
//     setPosition(pDriver, 0, 0);

//     for (i = 0; i < 8 * OLED_WIDTH; i++)
//         OLED_sendData(pDriver, data);

//     //更新缓存
//     memset(pDriver->buffer, data, sizeof(pDriver->buffer));
//     memset(pDriver->bufferFlag, 0, sizeof(pDriver->bufferFlag));
// }

// /**
//  * @description: 区域刷新
//  * @param {uint8_t} l   起始列地址
//  * @param {uint8_t} r   结束列地址
//  * @return {*}
//  */
// static void areaRefresh(OLED* const pDriver, uint8_t l, uint8_t r) {
//     uint8_t count = 0, i, up = 0, down = 7, x, page;

//     for (i = l; i <= r; i++) count |= pDriver->bufferFlag[i];

//     while (!(count & (1 << up))) up++;

//     while (!(count & (1 << down))) down--;

//     setVerticalPosition(pDriver, l, up, down);

//     for (x = l; x <= r; x++)
//         for (page = up; page <= down; page++)
//             OLED_sendData(pDriver, pDriver->buffer[page][x]);

//     memset(pDriver->bufferFlag + l, 0, r - l + 1);
// }

// /**
//  * @description: 全屏刷新
//  * @param {*}
//  * @return {*}
//  */
// void refresh(OLED* const pDriver) {
//     uint8_t l = 0, r;

//     while (l <= OLED_WIDTH) {
//         while (l <= OLED_WIDTH && !pDriver->bufferFlag[l]) l++;

//         r = l;
//         while (r <= OLED_WIDTH && pDriver->bufferFlag[r]) r++;
//         r--;

//         if (l <= OLED_WIDTH) areaRefresh(pDriver, l, r);

//         l = r + 1;
//     }
// }

// /**
//  * @description: 提取指定长度的二进制数据
//  * @param {uint8_t} *data 提取源
//  * @param {uint8_t} start 起始位置
//  * @param {uint8_t} size  提取长度
//  * @return {uint8_t}      最长为8位的结果
//  */
// static uint8_t getInt8Data(uint8_t *data, uint8_t start, uint8_t size) {
//     uint8_t ret = 0, delta;

//     ret = data[start / 8] >> (start % 8);
//     delta = 8 - (start % 8);
//     size -= delta;
//     start += delta;

//     if (((int8_t)size) <= 0)
//         return ret & (0xFF >> (-((int8_t)size)));
//     else
//         return ret | (getInt8Data(data, start, size) << delta);
// }

// /**
//  * @description: 从指定像素点向下绘制不定长一列
//  * @param {uint8_t} x   像素点横坐标
//  * @param {uint8_t} y   像素点纵坐标
//  * @param {uint8_t} *data   绘制数据源
//  * @param {uint8_t} size    绘制长度
//  * @param {uint8_t} bool    布尔计算类型
//  * @return {HAL_StatusTypeDef}  HAL状态
//  */
// HAL_StatusTypeDef drawColumn(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t *data,
//                                        uint8_t size, uint8_t bool) {
//     if (x >= OLED_WIDTH || y + size > OLED_HEIGHT) return HAL_ERROR;

//     uint8_t pos = 0;
//     while (size > 0) {
//         uint8_t data_n = pDriver->buffer[y / 8][x], delta;

//         if (bool == OLED_BOOL_Replace)
//             if (8 - (y % 8) >= size) {
//                 data_n = data_n - (data_n & ((0xff & (0xff << (8 - size))) >>
//                                              (8 - size - (y % 8))));
//                 data_n |= getInt8Data(data, pos, size) << (y % 8);
//                 delta = size;
//             } else {
//                 data_n = data_n - (data_n & (0xff << (y % 8)));
//                 data_n |= getInt8Data(data, pos, 8 - (y % 8)) << (y % 8);
//                 delta = 8 - (y % 8);
//             }
//         else if (bool == OLED_BOOL_ADD)
//             if (8 - (y % 8) >= size) {
//                 data_n |= getInt8Data(data, pos, size) << (y % 8);
//                 delta = size;
//             } else {
//                 data_n |= getInt8Data(data, pos, 8 - (y % 8)) << (y % 8);
//                 delta = 8 - (y % 8);
//             }
//         else if (bool == OLED_BOOL_Subtract)
//             if (8 - (y % 8) >= size) {
//                 data_n &= ~(getInt8Data(data, pos, size) << (y % 8));
//                 delta = size;
//             } else {
//                 data_n &= ~(getInt8Data(data, pos, 8 - (y % 8)) << (y % 8));
//                 delta = 8 - (y % 8);
//             }

//         if (data_n != pDriver->buffer[y / 8][x])
//             pDriver->bufferFlag[x] |= 1 << (y / 8);
//         pDriver->buffer[y / 8][x] = data_n;

//         size -= delta;
//         y += delta;
//         pos += delta;
//     }

//     return HAL_OK;
// }

// /**
//  * @description: 从指定像素点向下绘制不定长一列
//  * @param {uint8_t} x   像素点横坐标
//  * @param {uint8_t} y   像素点纵坐标
//  * @param {uint8_t} *data   绘制数据源
//  * @param {uint8_t} size    绘制长度
//  * @return {HAL_StatusTypeDef}  HAL状态
//  */
// HAL_StatusTypeDef replaceColumn(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t *data,
//                                   uint8_t size) {
//     return drawColumn(pDriver, x, y, data, size, OLED_BOOL_Replace);
// }

// /**
//  * @description: 以指定像素点为左上角，绘制单个字符
//  * @param {uint8_t} x   横坐标
//  * @param {uint8_t} y   纵坐标
//  * @param {uint8_t} c   字符
//  * @param {uint8_t} bool    布尔计算类型
//  * @return {HAL_StatusTypeDef}  HAL 状态
//  */
// HAL_StatusTypeDef drawChar(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t c, uint8_t bool) {
//     return drawBitmap(pDriver, x, y, 6, 8, ((uint8_t *)ASCII) + 6 * (c - 32),
//                              bool);
// }

// /**
//  * @description: 以指定像素点为左上角，绘制单个字符
//  * @param {uint8_t} x   横坐标
//  * @param {uint8_t} y   纵坐标
//  * @param {uint8_t} c   字符
//  * @return {HAL_StatusTypeDef}  HAL 状态
//  */
// HAL_StatusTypeDef replaceChar(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t c) {
//     return drawBitmap(pDriver, x, y, 6, 8, ((uint8_t *)ASCII) + 6 * (c - 32),
//                              OLED_BOOL_Replace);
// }
// /**
//  * @description: 以指定像素点为左上角，绘制字符串
//  * @param {uint8_t} x   横坐标
//  * @param {uint8_t} y   纵坐标
//  * @param {uint8_t} *str    字符串
//  * @param {uint8_t} bool    布尔计算类型
//  * @return {HAL_StatusTypeDef}  HAL 状态
//  */
// HAL_StatusTypeDef drawString(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t *str, uint8_t bool) {
//     uint16_t i = 0;

//     uint8_t ret = 0;

//     while (str[i] != '\0') {
//         if (x + 5 >= OLED_WIDTH) {
//             x = 0;
//             y += 8;
//         }

//         ret |= drawChar(pDriver, x, y, str[i], bool);

//         x += 6;
//         i++;
//     }

//     return ret ? HAL_ERROR : HAL_OK;
// }

// /**
//  * @description: 以指定像素点为左上角，绘制字符串
//  * @param {uint8_t} x   横坐标
//  * @param {uint8_t} y   纵坐标
//  * @param {uint8_t} *str    字符串
//  * @return {HAL_StatusTypeDef}  HAL 状态
//  */
// HAL_StatusTypeDef replaceString(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t *str) {
//     return drawString(pDriver, x, y, str, OLED_BOOL_Replace);
// }

// /**
//  * @description: 绘制位图
//  * @param {uint8_t} x   横坐标
//  * @param {uint8_t} y   纵坐标
//  * @param {uint8_t} width   宽度
//  * @param {uint8_t} height    高度
//  * @param {uint8_t} *data   数据
//  * @param {uint8_t} bool    布尔计算类型
//  * @return {HAL_StatusTypeDef}  HAL状态
//  */
// HAL_StatusTypeDef drawBitmap(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t *data, uint8_t bool) {
//     uint8_t ret = 0;
//     while (width > 0) {
//         ret |= drawColumn(pDriver, x, y, data, height, bool);
//         data += (height + 7) / 8;
//         width--;
//         x++;
//     }

//     return ret ? HAL_ERROR : HAL_OK;
// }

// /**
//  * @description: 绘制位图
//  * @param {uint8_t} x   横坐标
//  * @param {uint8_t} y   纵坐标
//  * @param {uint8_t} width   宽度
//  * @param {uint8_t} height    高度
//  * @param {uint8_t} *data   数据
//  * @return {HAL_StatusTypeDef}  HAL状态
//  */
// HAL_StatusTypeDef replaceBitmap(OLED* const pDriver, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t *data) {
//     return drawBitmap(pDriver, x, y, width, height, data, OLED_BOOL_Replace);
// }
