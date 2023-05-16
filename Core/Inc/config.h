#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"
#include "spi.h"

/**
 * 系统配置
 */

#define FIREWARE_VERSION "1.0.0"
#define DOG_FEED_INTERVAL 6000  //喂狗间隔时间（毫秒）
// #define DELAY_US_TIMER  &htim//微秒级延迟时钟

/**
 * OLED屏幕配置 
 */
#define OLED_WIDTH 128  //屏幕宽度（像素）
#define OLED_HEIGHT 64  //屏幕高度（像素）
#define OLED_CONTRAST 255  //屏幕对比度（0-255）
#define OLED_INVERSE_COLOR FALSE  //是否反色显示
#define OLED_ADDRESSING_MODE 1  //寻址模式 0 水平 1 垂直 2 复位
#define OLED_HI2C hi2c1 //I2C句柄
#define OLED_I2C_ADDRESS 0x78  //I2C设备地址
#define OLED_I2C_CLOCK_SPEED 400  //I2C时钟周期（KHZ）

/**
 * X轴电机配置
 */
#define X_MOTOR_IHOLD 1.2  //静止电流
#define X_MOTOR_IRUN 1.7  //最大电流
#define X_MOTOR_IHOLDDELAY 6  //降电流延时周期数 0-15
#define X_MOTOR_HSPI hspi2  //SPI句柄
#define X_MOTOR_CS_GPIO_PORT GPIOB  //片选GPIO
#define X_MOTOR_CS_PIN GPIO_PIN_12  //片选GPIO PIN

/**
 * Y轴电机配置
 */
#define Y_MOTOR_IHOLD 1.2  //静止电流
#define Y_MOTOR_IRUN 1.7  //最大电流
#define Y_MOTOR_IHOLDDELAY 6  //降电流延时周期数 0-15
#define Y_MOTOR_HSPI hspi1  //SPI句柄
#define Y_MOTOR_CS_GPIO_PORT GPIOA  //片选GPIO
#define Y_MOTOR_CS_PIN GPIO_PIN_4  //片选GPIO PIN

/**
 * 串口配置
 */
#define UPPER_COMPUTER_SERIAL_PORT_HUART huart1  //上位机串口UART句柄
#define UPPER_COMPUTER_SERIAL_PORT_BUFFER_MAX_SIZE 128  //上位机串口缓冲区最大大小
#define DEBUG_SERIAL_PORT_HUART huart2  //调试串口UART句柄
#define DEBUG_SERIAL_PORT_BUFFER_MAX_SIZE 128  //调试串口缓冲区最大大小
#define EXTENDS_SERIAL_PORT_HUART huart3  //扩展串口UART句柄
#define EXTENDS_SERIAL_PORT_BUFFER_MAX_SIZE 128  //扩展串口缓冲区最大大小

#endif
