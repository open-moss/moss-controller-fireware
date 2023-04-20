/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "spi.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "config.h"
#include "tmc5160.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId mainLoopWorkerHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartMainTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of mainLoopWorker */
  osThreadDef(mainLoopWorker, StartMainTask, osPriorityNormal, 0, 256);
  mainLoopWorkerHandle = osThreadCreate(osThread(mainLoopWorker), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartMainTask */
/**
  * @brief  Function implementing the mainLoopWorker thread.
  * @param  argument: Not used
  * @retval None
  */

void SendCmd(uint8_t address, uint32_t data) {
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
	uint8_t tbuf[5];
    tbuf[0] = address | 0x80;
    tbuf[1] = 0xFF & (data>>24);
    tbuf[2] = 0xFF & (data>>16);
    tbuf[3] = 0xFF & (data>>8);
    tbuf[4] = 0xFF & data;
	uint8_t rxbuf[5];
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi2, tbuf, rxbuf, 5, 100);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	Debug_HEX_Print((uint8_t *)rxbuf, sizeof(rxbuf), __FUNCTION__, __LINE__);
}

/* USER CODE END Header_StartMainTask */
void StartMainTask(void const * argument)
{
  /* USER CODE BEGIN StartMainTask */
	
	Debug_Printf("Hello World", __FUNCTION__, __LINE__);

  /* Infinite loop */
	
  MOTOR* motor = new_MOTOR(X_MOTOR_CS_GPIO_PORT, X_MOTOR_CS_PIN);
  motor->test();

	osDelay(3000);
	SendCmd(TMC5160_CHOPCONF, 0x000100C3);
	SendCmd(TMC5160_IHOLD_IRUN, 0x0006110E);
	SendCmd(TMC5160_TPOWERDOWN, 0x0000000A);
  SendCmd(TMC5160_TCOOLTHRS, 0x00000004);
  SendCmd(TMC5160_TSTEP, 0x00000004);
	SendCmd(TMC5160_GCONF, 0x00000004);
	SendCmd(TMC5160_TPWMTHRS, 0x000001F4);  //对应切换速度
	SendCmd(TMC5160_A1, 0x000003E8);  //第一阶段加�?�度
	SendCmd(TMC5160_V1, 0x0000C350);  //加�?�度阈�?��?�度
	SendCmd(TMC5160_AMAX, 0x000001F4);  //大于V1的加速度
	SendCmd(TMC5160_VMAX, 0x00030D40);  //加�?�度阈�?��?�度�?大�?�度
	SendCmd(TMC5160_DMAX, 0x000002BC);  //大于V1的减速度
	SendCmd(TMC5160_D1, 0x00000578);  //小于V1的减速度
	SendCmd(TMC5160_VSTOP, 0x0000000A);  //停止速度
	SendCmd(TMC5160_RAMPMODE, 0x00000000);  //目标位置运动
  SendCmd(TMC5160_XTARGET, 0);  //运动角度
  for(;;)
  {
		// osDelay(500);
	  // SendCmd(0xAD, i);
    // i += 2000;
		// printf("GOGOGO");
    osDelay(1);
  }
  /* USER CODE END StartMainTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
