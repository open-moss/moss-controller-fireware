#ifndef CONFIG_H
#define CONFIG_H

/**
 * 系统配置
 */

#define FIREWARE_VERSION "1.0.0"
#define DOG_FEED_INTERVAL 6000  //喂狗间隔时间（毫秒）

/**
 * X轴电机配置
 */
#define X_MOTOR_IHOLD 1.2  //静止电流
#define X_MOTOR_IRUN 1.7  //最大电流
#define X_MOTOR_IHOLDDELAY 6  //降电流延时周期数 0-15
#define X_MOTOR_CS_GPIO_PORT GPIOB  //片选GPIO
#define X_MOTOR_CS_PIN GPIO_PIN_12  //片选GPIO PIN

/**
 * Y轴电机配置
 */
#define Y_MOTOR_IHOLD 1.2  //静止电流
#define Y_MOTOR_IRUN 1.7  //最大电流
#define Y_MOTOR_IHOLDDELAY 6  //降电流延时周期数 0-15
#define Y_MOTOR_CS_GPIO_PORT GPIOA  //片选GPIO
#define Y_MOTOR_CS_PIN GPIO_PIN_4  //片选GPIO PIN

#endif