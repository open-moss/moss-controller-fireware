#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "tim.h"

/**
 * 系统配置
 */

#define FIREWARE_VERSION "1.0.0"
#define DOG_FEED_INTERVAL 6000  //喂狗间隔时间（毫秒）
#define DELAY_US_TIMER  htim2  //微秒级延迟时钟
#define DEBUG_MODE TRUE  //是否开启调试模式

/**
 * 设备配置 
 */
#define POWER3V3_GPIO_PORT GPIOC  //3V3电源GPIO
#define POWER3V3_PIN GPIO_PIN_9 //3V3电源GPIO PIN
#define POWER5V_GPIO_PORT GPIOC  //5V电源GPIO
#define POWER5V_PIN GPIO_PIN_8  //5V电源GPIO PIN
#define LASER_GPIO_PORT GPIOC  //激光灯GPIO
#define LASER_PIN GPIO_PIN_2  //激光灯GPIO_PIN
#define DATALIGHT_GPIO_PORT GPIOC  //数据灯GPIO
#define DATALIGHT_PIN GPIO_PIN_3  //数据灯GPIO PIN
#define STATUS_RED_LED_GPIO_PORT GPIOC  //红状态灯GPIO
#define STATUS_RED_LED_PIN GPIO_PIN_5  //红状态灯GPIO PIN
#define STATUS_BLUE_LED_GPIO_PORT GPIOB  //蓝状态灯GPIO
#define STATUS_BLUE_LED_PIN GPIO_PIN_0  //蓝状态灯GPIO PIN

/**
 * OLED屏幕配置 
 */
#define OLED_INVERSE_COLOR FALSE  //是否反色显示
#define OLED_TEXT_BUFFER_SIZE 128  //文本缓冲区大小
#define OLED_TEXT_BUFFER_COUNT 10  //文本缓冲区个数
#define OLED_HI2C hi2c1  //I2C句柄
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
#define X_MOTOR_LIMIT_GPIO_PORT GPIOC  //限位GPIO
#define X_MOTOR_LIMIT_PIN GPIO_PIN_7  //限位GPIO PIN

/**
 * Y轴电机配置
 */
#define Y_MOTOR_IHOLD 1.2  //静止电流
#define Y_MOTOR_IRUN 1.7  //最大电流
#define Y_MOTOR_IHOLDDELAY 6  //降电流延时周期数 0-15
#define Y_MOTOR_HSPI hspi1  //SPI句柄
#define Y_MOTOR_CS_GPIO_PORT GPIOA  //片选GPIO
#define Y_MOTOR_CS_PIN GPIO_PIN_4  //片选GPIO PIN
#define Y_MOTOR_LIMIT_GPIO_PORT GPIOA  //限位GPIO
#define Y_MOTOR_LIMIT_PIN GPIO_PIN_8  //限位GPIO PIN

/**
 * 串口配置
 */
#define UPPER_COMPUTER_SERIAL_PORT_HUART huart1  //上位机串口UART句柄
#define UPPER_COMPUTER_SERIAL_PORT_BUFFER_SIZE 128  //上位机串口缓冲区大小
#define UPPER_COMPUTER_SERIAL_PORT_TEMP_BUFFER_COUNT 5  //上位机串口临时缓冲区个数
#define UPPER_COMPUTER_SERIAL_PORT_TX_TIMEOUT 2000  //上位机串口接收数据超时时间
#define UPPER_COMPUTER_SERIAL_PORT_RX_TIMEOUT 2000  //上位机串口接收数据超时时间
#define DEBUG_SERIAL_PORT_HUART huart2  //调试串口UART句柄
#define DEBUG_SERIAL_PORT_BUFFER_MAX_SIZE 128  //调试串口缓冲区最大大小
#define EXTENDS_SERIAL_PORT_HUART huart3  //扩展串口UART句柄
#define EXTENDS_SERIAL_PORT_BUFFER_MAX_SIZE 128  //扩展串口缓冲区最大大小

/**
 * 激光测距配置
 */
#define TOF_HI2C hi2c2  //I2C句柄
#define TOF_I2C_ADDRESS 0x78  //I2C设备地址

#endif
