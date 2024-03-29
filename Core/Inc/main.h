/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "common.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LASER_LIGHT_Pin GPIO_PIN_2
#define LASER_LIGHT_GPIO_Port GPIOC
#define DATA_LIGHT_Pin GPIO_PIN_3
#define DATA_LIGHT_GPIO_Port GPIOC
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define STATUS_RED_Pin GPIO_PIN_5
#define STATUS_RED_GPIO_Port GPIOC
#define STATUS_BLUE_Pin GPIO_PIN_0
#define STATUS_BLUE_GPIO_Port GPIOB
#define _850NM_LIGHT_Pin GPIO_PIN_1
#define _850NM_LIGHT_GPIO_Port GPIOB
#define SPI2_NSS_Pin GPIO_PIN_12
#define SPI2_NSS_GPIO_Port GPIOB
#define IR_38KHZ_Pin GPIO_PIN_6
#define IR_38KHZ_GPIO_Port GPIOC
#define M2_LIMIT_Pin GPIO_PIN_7
#define M2_LIMIT_GPIO_Port GPIOC
#define OUT_5V_Pin GPIO_PIN_8
#define OUT_5V_GPIO_Port GPIOC
#define OUT_3V3_Pin GPIO_PIN_9
#define OUT_3V3_GPIO_Port GPIOC
#define M1_LIMIT_Pin GPIO_PIN_8
#define M1_LIMIT_GPIO_Port GPIOA
#define DATA_LIGHT_DT_Pin GPIO_PIN_4
#define DATA_LIGHT_DT_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
