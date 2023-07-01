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
#include "stdio.h"
#include "config.h"
#include "motor.h"
#include "oled.h"
#include "humiture_sensor.h"
#include "body_sensor.h"
#include "logger.h"
#include "messager.h"
#include "protocol.h"
#include "device.h"
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
Messager_Handle *pmgr;

MOTOR_Handle *pmotorX;
MOTOR_Handle *pmotorY;

HumitureSensor_Handle* phs;
BodySensor_Handle* pbs;

OLED_Handle *poled;

SemaphoreHandle_t printSemaphoreHandle = NULL;

/* USER CODE END Variables */
osThreadId mainLoopWorkerHandle;
osMessageQId serialDataQueueHandle;
osMessageQId oledDataQeueHandle;
osMessageQId bodySensorDataQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void MOTOR_HandleTask(void);
void OLED_HandleTask(void);
void EnvCollection_HandleTask(void);
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

  /* definition and creation of oledDataQeue */
  osMessageQDef(oledDataQeue, 10, uint32_t);
  oledDataQeueHandle = osMessageCreate(osMessageQ(oledDataQeue), NULL);

  /* definition and creation of bodySensorDataQueue */
  osMessageQDef(bodySensorDataQueue, 10, uint32_t);
  bodySensorDataQueueHandle = osMessageCreate(osMessageQ(bodySensorDataQueue), NULL);

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

  //创建消息处理任务
  while (CreateTask("Message_HandleTask", Message_HandleTask, osPriorityHigh, 256) != HAL_OK)
  {
    LogInfo("Message Handle Task Create Failed");
    osDelay(100);
  }
  LogInfo("Message Handle Task Created");

  //创建OLED处理任务
  while (CreateTask("OLED_HandleTask", OLED_HandleTask, osPriorityBelowNormal, 256) != HAL_OK)
  {
    LogInfo("OLED Handle Task Create Failed");
    osDelay(100);
  }
  LogInfo("OLED Handle Task Created");

  //创建电机处理任务
  while (CreateTask("Motor_HandleTask", MOTOR_HandleTask, osPriorityBelowNormal, 192) != HAL_OK)
  {
    LogInfo("MOTOR Handle Task Create Failed");
    osDelay(100);
  }
  LogInfo("Motor Handle Task Created");  

  //创建激光测距处理任务
  while (CreateTask("EnvCollection_HandleTask", EnvCollection_HandleTask, osPriorityIdle, 768) != HAL_OK)
  {
    LogInfo("EnvCollection Handle Task Create Failed");
    osDelay(100);
  }
  LogInfo("EnvCollection Handle Task Created");

  Device_DatalightOpen();  //初始化完毕后开启数据灯

  // DataPacket *pdata = Protocol_BuildDataPacket(DATA_PACKET_HEARTBEAT, "123456", sizeof("123456"));
  // DataPacket *pdata1 = Protocol_BufferToDataPacket(Protocol_DataPacketToBuffer(pdata));
  // PrintHEX(Protocol_DataPacketToBuffer(pdata1), pdata->size);


  for (;;)
  {
    // uint8_t data[30];
    // HAL_UART_Receive(&huart5, data, 30, 5000);
    // PrintHEX(data, sizeof(data));
    // PrintTaskList();
    // LogInfo("Test");
    osDelay(5000);
    OLED_PushString(poled, "HelloWorld");
  }
  /* USER CODE END StartMainTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void MOTOR_HandleTask()
{
  pmotorX = MOTOR_Init(&X_MOTOR_HSPI, X_MOTOR_CS_GPIO_PORT, X_MOTOR_CS_PIN, X_MOTOR_LIMIT_GPIO_PORT, X_MOTOR_LIMIT_PIN, X_MOTOR_IRUN, X_MOTOR_IHOLD, X_MOTOR_IHOLDDELAY);
  pmotorY = MOTOR_Init(&Y_MOTOR_HSPI, Y_MOTOR_CS_GPIO_PORT, Y_MOTOR_CS_PIN, Y_MOTOR_LIMIT_GPIO_PORT, Y_MOTOR_LIMIT_PIN, Y_MOTOR_IRUN, Y_MOTOR_IHOLD, Y_MOTOR_IHOLDDELAY);

  // MOTOR_Rotate(&pmotorY, 51200 * 20);
  for (;;)
  {
    if (MOTOR_LimitCheck(pmotorX))
      LogInfo("X MOTOR LIMIT!");
    if (MOTOR_LimitCheck(pmotorY))
      LogInfo("Y MOTOR LIMIT!");
    // if(MOTOR_GetRotateAngle(pmotorX) >= 360)
    //   MOTOR_Rotate(pmotorX, -10);
    // else if(MOTOR_GetRotateAngle(pmotorX) <= 0)
    //   MOTOR_Rotate(pmotorX, 370);
    // if(MOTOR_GetRotateAngle(pmotorY) >= 360)
    //   MOTOR_Rotate(pmotorY, -10);
    // else if(MOTOR_GetRotateAngle(pmotorY) <= 0)
    //   MOTOR_Rotate(pmotorY, 370);
    osDelay(500);
  }
}

void OLED_HandleTask()
{
  poled = OLED_Init(&OLED_HI2C, oledDataQeueHandle, OLED_TEXT_FONT, OLED_TEXT_MAX_WIDTH, OLED_TEXT_MAX_HEIGHT, OLED_TEXT_CHAR_WIDTH, OLED_TEXT_CHAR_HEIGHT, OLED_TEXT_BUFFER_LENGTH, OLED_TEXT_BUFFER_COUNT, OLED_TEXT_TYPING_INTERVAL);
  for (;;)
  {

    // OLED_PartClear(poled, 0, 24, 128, 24);
    // uint8_t str1[20];
    // uint8_t str2[20];
    // memset(str1, 0, sizeof(uint8_t) * 20);
    // memset(str2, 0, sizeof(uint8_t) * 20);
    // sprintf((char *)str1, "X MOTOR: %d", MOTOR_GetRotateAngle(pmotorX));
    // sprintf((char *)str2, "Y MOTOR: %d", MOTOR_GetRotateAngle(pmotorY));
    // OLED_DrawString(poled, 0, 36, str1);
    // OLED_DrawString(poled, 0, 48, str2);
    // OLED_Refresh(poled);
    OLED_MessageHandle(poled);
    // osDelay(500);
  }
}

volatile int IntCount;
#define isAutonomousExample 1 /* Allow to select either autonomous ranging or fast ranging example */
#define isInterrupt 1         /* If isInterrupt = 1 then device working in interrupt mode, else device working in polling mode */

void EnvCollection_HandleTask()
{
  phs = HumitureSensor_Init(&HUMITURE_SENSOR_HI2C);
  pbs = BodySensor_Init(&BODY_SENSOR_SERIAL_PORT_HUART, bodySensorDataQueueHandle, BODY_SENSOR_SERIAL_PORT_TEMP_BUFFER_COUNT, BODY_SENSOR_SERIAL_PORT_RX_TIMEOUT);
  //if (BodySensor_Listen(pbs) != HAL_OK)
  //  LogError("BodySensor Listen Failed");
  while(1) {
    HumitureSensor_MeasureData *measureData = HumitureSensor_Measuring(phs);
    if(measureData == NULL) {
      LogWarn("HumitureSensor Measuring Failed");
      HumitureSensor_Reset(phs);
      osDelay(500);
      continue;
    }
    uint8_t str1[30];
    uint8_t str2[30];
    memset(str1, 0, sizeof(uint8_t) * 30);
    memset(str2, 0, sizeof(uint8_t) * 30);
    sprintf((char *)str1, "Temperature: %2.2f", measureData->temperature);
    sprintf((char *)str2, "Humidity: %2.2f", measureData->humidity);
    HumitureSensor_FreeMeasureData(measureData);
    OLED_PushString(poled, str1);
    // osDelay(100);
    OLED_PushString(poled, str2);
    // OLED_Refresh(poled);
    // BodySensor_Measuring(pbs);
    osDelay(1000);
  }
}

void Message_HandleTask()
{
  pmgr = Messager_Init(&UPPER_COMPUTER_SERIAL_PORT_HUART, serialDataQueueHandle, UPPER_COMPUTER_SERIAL_PORT_BUFFER_SIZE, UPPER_COMPUTER_SERIAL_PORT_TEMP_BUFFER_COUNT, UPPER_COMPUTER_SERIAL_PORT_TX_TIMEOUT, UPPER_COMPUTER_SERIAL_PORT_RX_TIMEOUT);
  if (Messager_Listen(pmgr) != HAL_OK)
    LogError("Messager Listen Failed");
  while (1) {
    Messager_MessageHandle(pmgr);
    // osDelay(100);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == pmgr->huart->Instance)
    Messager_TxCpltCallback(pmgr);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == pmgr->huart->Instance)
    Messager_RxCpltCallback(pmgr);
  if (huart->Instance == pbs->huart->Instance)
    BodySensor_RxCpltCallback(pbs);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
