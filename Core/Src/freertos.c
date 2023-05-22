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
#include "messager.h"
#include "protocol.h"
#include "vl53l1_api.h"
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
MESSAGER_Handle *pmgr;

MOTOR_Handle *pmotorX;
MOTOR_Handle *pmotorY;

OLED_Handle *poled;

SemaphoreHandle_t printSemaphoreHandle = NULL;

uint8_t rxFlag = FALSE;
/* USER CODE END Variables */
osThreadId mainLoopWorkerHandle;
osMessageQId serialDataQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void MOTOR_HandleTask(void);
void OLED_HandleTask(void);
void ToF_HandleTask(void);
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

  /* Create the queue(s) */
  /* definition and creation of serialDataQueue */
  osMessageQDef(serialDataQueue, 10, uint32_t);
  serialDataQueueHandle = osMessageCreate(osMessageQ(serialDataQueue), NULL);

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

  while (createTask("MOTOR_HandleTask", MOTOR_HandleTask, osPriorityBelowNormal, 192) != HAL_OK)
  {
    DebugPrintf("MOTOR Handle Task Create Failed", __FUNCTION__, __LINE__);
    osDelay(100);
  }

  DebugPrintf("MOTOR Handle Task Created", __FUNCTION__, __LINE__);  

  while (createTask("ToF_HandleTask", ToF_HandleTask, osPriorityBelowNormal, 768) != HAL_OK)
  {
    DebugPrintf("ToF Handle Task Create Failed", __FUNCTION__, __LINE__);
    osDelay(100);
  }

  DebugPrintf("ToF Handle Task Created", __FUNCTION__, __LINE__);

  while (createTask("Message_HandleTask", Message_HandleTask, osPriorityHigh, 256) != HAL_OK)
  {
    DebugPrintf("Message Handle Task Create Failed", __FUNCTION__, __LINE__);
    osDelay(100);
  }

  while (createTask("OLED_HandleTask", OLED_HandleTask, osPriorityBelowNormal, 256) != HAL_OK)
  {
    DebugPrintf("OLED Handle Task Create Failed", __FUNCTION__, __LINE__);
    osDelay(100);
  }

  DebugPrintf("OLED Handle Task Created", __FUNCTION__, __LINE__);

  DataPacket* pData = Protocol_BuildDataPacket(Heartbeat, "65535", sizeof("65535"));
  DebugHEXPrint(Protocol_DataPacketToBuffer(pData), pData->size, __FUNCTION__, __LINE__);

  for (;;)
  {
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_SET)
      printf("FOUNDED HUMAN!!!");
    // PrintTaskList();
    osDelay(1000);
  }
  /* USER CODE END StartMainTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void MOTOR_HandleTask()
{
  pmotorX = MOTOR_Init(&X_MOTOR_HSPI, X_MOTOR_CS_GPIO_PORT, X_MOTOR_CS_PIN, X_MOTOR_LIMIT_GPIO_PORT, X_MOTOR_LIMIT_PIN);
  pmotorY = MOTOR_Init(&Y_MOTOR_HSPI, Y_MOTOR_CS_GPIO_PORT, Y_MOTOR_CS_PIN, Y_MOTOR_LIMIT_GPIO_PORT, Y_MOTOR_LIMIT_PIN);
  // MOTOR_Rotate(pmotorX, 360);
  // MOTOR_Rotate(pmotorY, 360);
  // MOTOR_Rotate(&pmotorY, 51200 * 20);
  for (;;)
  {
    if (MOTOR_LimitCheck(pmotorX))
      DebugPrintf("X MOTOR LIMIT!", __FUNCTION__, __LINE__);
    if (MOTOR_LimitCheck(pmotorY))
      DebugPrintf("Y MOTOR LIMIT!", __FUNCTION__, __LINE__);
    osDelay(500);
  }
}

void OLED_HandleTask()
{
  poled = OLED_Init(&OLED_HI2C);
  OLED_DrawString(poled, 0, 10, "OpenMOSS");
  OLED_DrawString(poled, 0, 20, "BOOT (FIREWARE.bin)");
  OLED_Refresh(poled);
  for (;;)
  {
    uint8_t str1[20];
    uint8_t str2[20];
    sprintf((char *)str1, "X MOTOR: %d", MOTOR_GetRotateAngle(pmotorX));
    sprintf((char *)str2, "Y MOTOR: %d", MOTOR_GetRotateAngle(pmotorY));
    OLED_DrawString(poled, 0, 30, str1);
    OLED_DrawString(poled, 0, 40, str2);
    OLED_Refresh(poled);
    osDelay(100);
  }
}

volatile int IntCount;
#define isAutonomousExample 1 /* Allow to select either autonomous ranging or fast ranging example */
#define isInterrupt 1         /* If isInterrupt = 1 then device working in interrupt mode, else device working in polling mode */

void ToF_HandleTask()
{
  printf("VL53L1X Examples...\n");
  VL53L1_Dev_t dev;
  VL53L1_DEV Dev = &dev;
  Dev->I2cHandle = &hi2c2;
  Dev->I2cDevAddr = 0x52;
  uint8_t byteData;
  uint16_t wordData;
  VL53L1_RdByte(Dev, 0x010F, &byteData);
  printf("VL53L1X Model_ID: %02X\n\r", byteData);
  VL53L1_RdByte(Dev, 0x0110, &byteData);
  printf("VL53L1X Module_Type: %02X\n\r", byteData);
  VL53L1_RdWord(Dev, 0x010F, &wordData);
  printf("VL53L1X: %02X\n\r", wordData);
  static VL53L1_RangingMeasurementData_t RangingData;
  printf("Autonomous Ranging Test\n");
  int status = VL53L1_WaitDeviceBooted(Dev);
  status = VL53L1_DataInit(Dev);
  status = VL53L1_StaticInit(Dev);
  status = VL53L1_SetDistanceMode(Dev, VL53L1_DISTANCEMODE_LONG);
  status = VL53L1_SetMeasurementTimingBudgetMicroSeconds(Dev, 50000);
  status = VL53L1_SetInterMeasurementPeriodMilliSeconds(Dev, 500);
  status = VL53L1_StartMeasurement(Dev);
  if (status)
  {
    printf("VL53L1_StartMeasurement failed \n");
    while (1);
  }
  for (;;)
  {
    status = VL53L1_WaitMeasurementDataReady(Dev);
    if (!status)
    {
      status = VL53L1_GetRangingMeasurementData(Dev, &RangingData);
      if (status == 0)
      {
        printf("%dmm\n", RangingData.RangeMilliMeter);
        MOTOR_Rotate(pmotorX, RangingData.RangeMilliMeter);
        MOTOR_Rotate(pmotorY, RangingData.RangeMilliMeter);
        // printf("%d,%d,%.2f,%.2f\n", RangingData.RangeStatus,RangingData.RangeMilliMeter,
        //         (RangingData.SignalRateRtnMegaCps/65536.0),RangingData.AmbientRateRtnMegaCps/65336.0);
      }
      status = VL53L1_ClearInterruptAndStartMeasurement(Dev);
    }
    osDelay(500);
  }
}

void Message_HandleTask()
{
  pmgr = MESSAGER_Init(&UPPER_COMPUTER_SERIAL_PORT_HUART, UPPER_COMPUTER_SERIAL_PORT_BUFFER_SIZE, UPPER_COMPUTER_SERIAL_PORT_RX_TIMEOUT);
  if (MESSAGER_Listen(pmgr) != HAL_OK)
    DebugPrintf("Messager Listen Failed", __FUNCTION__, __LINE__);
  while (1)
  {
    osDelay(100);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == pmgr->huart->Instance)
    MESSAGER_TxCpltCallback(pmgr);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == pmgr->huart->Instance)
    MESSAGER_RxCpltCallback(pmgr);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
