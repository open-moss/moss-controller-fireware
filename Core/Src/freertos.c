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
#include "config.h"
#include "spi.h"
#include "i2c.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"
#include "motor.h"
#include "oled.h"
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
SemaphoreHandle_t printSemaphoreHandle = NULL;
MOTOR_Handle hmotorX;
uint8_t rxData[1] = {0};
uint8_t rxFlag = FALSE;
/* USER CODE END Variables */
osThreadId mainLoopWorkerHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void MOTOR_HandleTask(void);
void OLED_HandleTask(void);
void Message_HandleTask(void);
/* USER CODE END FunctionPrototypes */

void StartMainTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
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
  printSemaphoreHandle = xSemaphoreCreateMutex();
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

/* USER CODE END Header_StartMainTask */
void StartMainTask(void const * argument)
{
  /* USER CODE BEGIN StartMainTask */


  /* Infinite loop */

  osDelay(100);

  DebugPrintf("Hello World", __FUNCTION__, __LINE__);

  while(createTask("MOTOR_HandleTask", MOTOR_HandleTask, osPriorityBelowNormal, 256) != HAL_OK) {
    DebugPrintf("MOTOR Handle Task Create Failed", __FUNCTION__, __LINE__);
    osDelay(100);
  }


  DebugPrintf("Hello World", __FUNCTION__, __LINE__);
  
  while(createTask("OLED_HandleTask", OLED_HandleTask, osPriorityBelowNormal, 768) != HAL_OK) {
    DebugPrintf("OLED Handle Task Create Failed", __FUNCTION__, __LINE__);
    osDelay(100);
  }
  
  DebugPrintf("Hello World", __FUNCTION__, __LINE__);

  while(createTask("Message_HandleTask", Message_HandleTask, osPriorityHigh, 256) != HAL_OK) {
    DebugPrintf("Message Handle Task Create Failed", __FUNCTION__, __LINE__);
    osDelay(100);
  }

  for (;;)
  {
    osDelay(1000);
  }
  /* USER CODE END StartMainTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void MOTOR_HandleTask() {
  hmotorX = MOTOR_Init(&X_MOTOR_HSPI, X_MOTOR_CS_GPIO_PORT, X_MOTOR_CS_PIN);
  MOTOR_Handle hmotorY = MOTOR_Init(&Y_MOTOR_HSPI, Y_MOTOR_CS_GPIO_PORT, Y_MOTOR_CS_PIN);
  MOTOR_Rotate(&hmotorX, 0);
  MOTOR_Rotate(&hmotorY, 0);
  // MOTOR_Rotate(&hmotorY, 51200 * 20);
  // osDelay(2000);
  // MOTOR_SendCommand(&hmotorX, TMC5160_SWMODE, 0x400);
  // osDelay(100);
  for (;;)
  {
    // DebugPrintf("%d", __FUNCTION__, __LINE__, MOTOR_GetRotateAngle(&hmotorX));
    // MOTOR_Rotate(&hmotorX, angle += 15);
    // MOTOR_ReadData(&hmotorX, TMC5160_XACTUAL);
    // MOTOR_Rotate(&hmotorX, rand() % 51058 + 142);
    // MOTOR_Rotate(&hmotorY, rand() % 51058 + 142);
    // MOTOR_Rotate(&hmotorX, rand());
    // MOTOR_ReadData(&hmotorX, TMC5160_XACTUAL);
    osDelay(500);
  }
}

void OLED_HandleTask() {
  OLED_Handle holed = OLED_Init(&OLED_HI2C);
  OLED_DrawString(&holed, 0, 10, "OpenMOSS");
  OLED_DrawString(&holed, 0, 20, "BOOT (FIREWARE.bin)");
  OLED_Refresh(&holed);
  for (;;)
  {
    osDelay(100);
  }
}

void Message_HandleTask() {
 while(1) {
  HAL_UART_Receive_IT(&huart2, rxData, 1);
  if(rxFlag) {
    rxFlag = FALSE;
    MOTOR_Rotate(&hmotorX, rxData[0]);
  }
  osDelay(100);
 }
}

/**
 * 串口发�?�回�??
 */
void UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  // DebugPrintfISR("PPPPPP", __FUNCTION__, __LINE__);
    if (huart->Instance == USART2)
    {
        
    }
}

/**
 * 串口接收回调
 */
void UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  // DebugPrintfISR("WWWWWWW", __FUNCTION__, __LINE__);
    if (huart->Instance == USART2)
    {
      // if(rxData[0] == 0x0D) {
      //   // DebugPrintfISR("OOOOOOK", __FUNCTION__, __LINE__);
      // }
      // while (HAL_UART_Receive_IT(&huart2, rxData, 1) != HAL_OK)
      // {
      //     osDelay(10);
      // }
    }
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  UART_TxCpltCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART2) {
    rxFlag = TRUE;
  }
  // UART_RxCpltCallback(huart);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
