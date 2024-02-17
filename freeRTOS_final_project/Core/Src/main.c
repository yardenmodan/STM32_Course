/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "string.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdlib.h"
#include "semphr.h"
#include <stdio.h>
#include "timers.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef enum {
    POLICE,
    FIRE,
    AMBULANCE,
    CORONA
}department_id;

typedef struct {
    uint8_t init_num;
    uint8_t concurrent_num;
    uint8_t available_num;

}department_data;

typedef struct {
    department_id dep_id;
    uint8_t requested_vehicles;
    uint32_t time_to_complete;
}request;

typedef struct {
    uint8_t available_num;
}dispatcher_data;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define DEPARTMENTS_NUM 4
#define MAX_NUM_REQUESTED_VEHICLES 10
#define MAX_SR_HANDLES 10 // max number of handles for sr threads in each department

#define INIT_POLICE_NUM 10
#define INIT_AMBULANCE_NUM 10
#define INIT_FIRE_NUM 10
#define INIT_CORONA_NUM 10
#define CORONA_CONCURRENT_NUM 10
#define AMBULANCE_CONCURRENT_NUM 10
#define POLICE_CONCURRENT_NUM 10
#define FIRE_CONCURRENT_NUM 10
#define INIT_DISPATCHER_SIZE 50
#define DISPATCH_INIT_BUFF_SIZE_BYTES 10
#define AMBULANCE_INIT_BUFF_SIZE_BYTES 10
#define FIRE_INIT_BUFF_SIZE_BYTES 10
#define CORONA_INIT_BUFF_SIZE_BYTES 10
#define POLICE_INIT_BUFF_SIZE_BYTES 10
#define LOG_TIMER_INTERVAL 1000
#define DISPATCH_QUEUE_TIMEOUT portMAX_DELAY
#define POLICE_QUEUE_TIMEOUT portMAX_DELAY
#define FIRE_QUEUE_TIMEOUT portMAX_DELAY
#define AMBULANCE_QUEUE_TIMEOUT portMAX_DELAY
#define CORONA_QUEUE_TIMEOUT portMAX_DELAY
#define LOG_TIMER_INTERVAL 1000
#define XTICKS_MAX 500
#define LOGGING_THREAD_PRIORITY (1)
#define LOG_THREAD_BUFFER_SIZE (128)

/* Define for GenerateThread */
#define GENERATE_THREAD_PRIORITY (2)
#define GENERATE_THREAD_BUFFER_SIZE (128)

/* Define for DispatchThread */
#define DISPATCH_THREAD_PRIORITY (2)
#define DISPATCH_THREAD_BUFFER_SIZE (128)

/* Define for PoliceThread */
#define POLICE_THREAD_PRIORITY (2)
#define POLICE_THREAD_BUFFER_SIZE (128)

/* Define for FireThread */
#define FIRE_THREAD_PRIORITY (2)
#define FIRE_THREAD_BUFFER_SIZE (128)

/* Define for AmbulanceThread */
#define AMBULANCE_THREAD_PRIORITY (2)
#define AMBULANCE_THREAD_BUFFER_SIZE (128)

/* Define for CoronaThread */
#define CORONA_THREAD_PRIORITY (2)
#define CORONA_THREAD_BUFFER_SIZE (128)

#define AMBULANCE_SR_THREAD_PRIORITY (2)
#define AMBULANCE_SR_THREAD_BUFFER_SIZE (128)

#define POLICE_SR_THREAD_PRIORITY (2)
#define POLICE_SR_THREAD_BUFFER_SIZE (128)

#define CORONA_SR_THREAD_PRIORITY (2)
#define CORONA_SR_THREAD_BUFFER_SIZE (128)

#define FIRE_SR_THREAD_PRIORITY (2)
#define FIRE_SR_THREAD_BUFFER_SIZE (128)

#define SEMAPHORE_WAIT_TIME portMAX_DELAY
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma location=0x2004c000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x2004c0a0
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x2004c000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x2004c0a0))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __GNUC__ ) /* GNU Compiler */
ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */

#endif

ETH_TxPacketConfig TxConfig;

ETH_HandleTypeDef heth;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */
uint32_t totalVehicles = MAX_NUM_REQUESTED_VEHICLES;


BaseType_t department_thread_priority[DEPARTMENTS_NUM] = { POLICE_SR_THREAD_PRIORITY, FIRE_SR_THREAD_PRIORITY, AMBULANCE_SR_THREAD_PRIORITY, CORONA_SR_THREAD_PRIORITY };
department_data police_dep = { INIT_POLICE_NUM, POLICE_CONCURRENT_NUM, POLICE_CONCURRENT_NUM };
department_data fire_dep = { INIT_FIRE_NUM, FIRE_CONCURRENT_NUM, FIRE_CONCURRENT_NUM };
department_data ambulance_dep = { INIT_AMBULANCE_NUM, AMBULANCE_CONCURRENT_NUM, AMBULANCE_CONCURRENT_NUM };
department_data corona_dep = { INIT_CORONA_NUM, CORONA_CONCURRENT_NUM, CORONA_CONCURRENT_NUM };
department_data department_list[DEPARTMENTS_NUM] = { 0 };
dispatcher_data dispatch_data = { INIT_DISPATCHER_SIZE };

SemaphoreHandle_t policeSemaphore;
SemaphoreHandle_t globalSemaphore;

SemaphoreHandle_t ambulanceSemaphore;
SemaphoreHandle_t fireSemaphore;
SemaphoreHandle_t coronaSemaphore;

/* Definitions for loggingThread */
TaskHandle_t loggingThreadHandle;


/* Definitions for GenerateThread */
TaskHandle_t generateThreadHandle;

/* Definitions for DispatchThread */


TaskHandle_t dispatchThreadHandle;


/* Definitions for PoliceThread */
TaskHandle_t policeThreadHandle;

/* Definitions for FireThread */
TaskHandle_t fireThreadHandle;


/* Definitions for AmbulanceThread */
TaskHandle_t ambulanceThreadHandle;


/* Definitions for CoronaThread */
TaskHandle_t coronaThreadHandle;

/* Definitions for DispatchQueue */
QueueHandle_t DispatchQueueHandle;

/* Definitions for AmbulanceQueue */
QueueHandle_t AmbulanceQueueHandle;

/* Definitions for PoliceQueue */
QueueHandle_t PoliceQueueHandle;

/* Definitions for FireQueue */
QueueHandle_t FireQueueHandle;

/* Definitions for CoronaQueue */
QueueHandle_t CoronaQueueHandle;


QueueHandle_t department_queue_handles_lists[DEPARTMENTS_NUM];
TickType_t QueueTimeoutList[DEPARTMENTS_NUM] = { POLICE_QUEUE_TIMEOUT,FIRE_QUEUE_TIMEOUT,AMBULANCE_QUEUE_TIMEOUT,CORONA_QUEUE_TIMEOUT };

//TaskFunction_t loggingStartThread(void* argument);
SemaphoreHandle_t semaphoreList[DEPARTMENTS_NUM];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
void StartDefaultTask(void *argument);



/* USER CODE BEGIN PFP */
TaskFunction_t dispatchTask(void* argument);
TaskFunction_t departmentTask(void* dep_id); //argument is speartment_id
int __io_putchar(int ch);
TaskFunction_t generateTask(void* argument);
void releaseResources(int i, department_id depar_id);
TaskFunction_t ServiceRoutine(void* req);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */
  department_list[POLICE] = police_dep;
  department_list[FIRE] = fire_dep;
  department_list[AMBULANCE] = ambulance_dep;
  department_list[CORONA] = corona_dep;

  policeSemaphore = xSemaphoreCreateCounting(
	  MIN(POLICE_CONCURRENT_NUM, INIT_POLICE_NUM),
	  MIN(POLICE_CONCURRENT_NUM, INIT_POLICE_NUM)
  );
  printf("police semaphore count: %d\r\n", uxSemaphoreGetCount(policeSemaphore));
  fireSemaphore = xSemaphoreCreateCounting(
	  MIN(FIRE_CONCURRENT_NUM, INIT_FIRE_NUM),
	  MIN(FIRE_CONCURRENT_NUM, INIT_FIRE_NUM)
  );
  printf("fire semaphore count: %d\r\n", uxSemaphoreGetCount(fireSemaphore));

  ambulanceSemaphore = xSemaphoreCreateCounting(
	  MIN(AMBULANCE_CONCURRENT_NUM, INIT_AMBULANCE_NUM),
	  MIN(AMBULANCE_CONCURRENT_NUM, INIT_AMBULANCE_NUM)
  );
  HAL_UART_Transmit(&huart3, "blabla\r\n", strlen("blabla\r\n"), HAL_MAX_DELAY);
  printf("ambulance semaphore count: %d\r\n", uxSemaphoreGetCount(ambulanceSemaphore));

  coronaSemaphore = xSemaphoreCreateCounting(
	  MIN(CORONA_CONCURRENT_NUM, INIT_CORONA_NUM),
	  MIN(CORONA_CONCURRENT_NUM, INIT_CORONA_NUM)
  );
  printf("corona semaphore count: %d\r\n", uxSemaphoreGetCount(coronaSemaphore));

  globalSemaphore = xSemaphoreCreateCounting(MAX_NUM_REQUESTED_VEHICLES,
	  MAX_NUM_REQUESTED_VEHICLES);
  printf("global semaphore count: %d\r\n", uxSemaphoreGetCount(globalSemaphore));


  semaphoreList[0] = policeSemaphore;
  semaphoreList[1] = fireSemaphore;

  semaphoreList[2] = ambulanceSemaphore;

  semaphoreList[3] = coronaSemaphore;



  /* Create the queue(s) */
  /* creation of DispatchQueue */
  DispatchQueueHandle = xQueueCreate(DISPATCH_INIT_BUFF_SIZE_BYTES, sizeof(request));

  /* creation of AmbulanceQueue */
  AmbulanceQueueHandle = xQueueCreate(AMBULANCE_INIT_BUFF_SIZE_BYTES, sizeof(request));

  /* creation of PoliceQueue */
  PoliceQueueHandle = xQueueCreate(POLICE_INIT_BUFF_SIZE_BYTES, sizeof(request));

  /* creation of FireQueue */
  FireQueueHandle = xQueueCreate(FIRE_INIT_BUFF_SIZE_BYTES, sizeof(request));

  /* creation of CoronaQueue */
  CoronaQueueHandle = xQueueCreate(CORONA_INIT_BUFF_SIZE_BYTES, sizeof(request));


  /* creation of loggingThread */
  //BaseType_t log_thread = xTaskCreate(loggingStartThread, (const char*)"loggingStartThread", (const void*)LOG_THREAD_BUFFER_SIZE, NULL, LOGGING_THREAD_PRIORITY, (const TaskHandle_t*)&loggingThreadHandle);
  department_id corona_depar = CORONA;
  department_id fire_depar = FIRE;
  department_id police_depar = POLICE;
  department_id ambulance_depar = AMBULANCE;

  /* USER CODE BEGIN RTOS_THREADS */
  BaseType_t generate_thread = xTaskCreate(generateTask, (const char * const)"generateThread", (const)GENERATE_THREAD_BUFFER_SIZE, NULL, GENERATE_THREAD_PRIORITY, (TaskHandle_t * const)&generateThreadHandle);
  BaseType_t dispatch_thread = xTaskCreate(dispatchTask, (const char * const)"dispatchThread", (const)DISPATCH_THREAD_BUFFER_SIZE, NULL, DISPATCH_THREAD_PRIORITY, (TaskHandle_t * const)&dispatchThreadHandle);

  BaseType_t police_thread = xTaskCreate(departmentTask, (const char * const)"policeThread", (const)POLICE_THREAD_BUFFER_SIZE, (void *)police_depar,(UBaseType_t)POLICE_THREAD_PRIORITY, (TaskHandle_t * const)&policeThreadHandle);

  BaseType_t fire_thread = xTaskCreate(departmentTask, (const char * const)"fireThread", (const)FIRE_THREAD_BUFFER_SIZE, (void *)fire_depar, (UBaseType_t)FIRE_THREAD_PRIORITY, (TaskHandle_t * const)&fireThreadHandle);
  BaseType_t ambulnace_thread = xTaskCreate(departmentTask, (const char * const)"ambulanceThread", (const)AMBULANCE_THREAD_BUFFER_SIZE, (void *)ambulance_depar, (UBaseType_t)AMBULANCE_THREAD_PRIORITY, (TaskHandle_t * const)&ambulanceThreadHandle);
  BaseType_t corona_thread = xTaskCreate(departmentTask, (const char * const)"coronaThread", (const)CORONA_THREAD_BUFFER_SIZE, (void *)corona_depar, (UBaseType_t)CORONA_THREAD_PRIORITY, (TaskHandle_t * const)&coronaThreadHandle);

/*const char * const pcName,	lint !e971 Unqualified char types are allowed for strings and single characters only.
							const configSTACK_DEPTH_TYPE usStackDepth,
							void * const pvParameters,
							UBaseType_t uxPriority,
							TaskHandle_t * const pxCreatedTask ) PRIVILEGED_FUNCTION;*/
  vTaskStartScheduler();
  /* USER CODE END 2 */

  /* Init scheduler */

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
  /* creation of defaultTask */

/*const char * const pcName,	lint !e971 Unqualified char types are allowed for strings and single characters only.
							const configSTACK_DEPTH_TYPE usStackDepth,
							void * const pvParameters,
							UBaseType_t uxPriority,
							TaskHandle_t * const pxCreatedTask ) PRIVILEGED_FUNCTION;*/
  /* USER CODE END 2 */

  /* Init scheduler */

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
  /* creation of defaultTask */

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */

TaskFunction_t dispatchTask(void* argument) {



    BaseType_t retval_Sent_From_Dispatch_Queue = 0;
    BaseType_t retval_Police_Send = 0;
    BaseType_t retval_Fire_Send = 0;
    BaseType_t retval_Corona_Send = 0;
    BaseType_t retval_Ambulance_Send = 0;

    BaseType_t retval_Send_to_dep_list[DEPARTMENTS_NUM] = { retval_Police_Send,retval_Fire_Send, retval_Ambulance_Send,retval_Corona_Send };
    department_queue_handles_lists[POLICE] = PoliceQueueHandle;
    department_queue_handles_lists[FIRE] = FireQueueHandle;
    department_queue_handles_lists[AMBULANCE] = AmbulanceQueueHandle;
    department_queue_handles_lists[CORONA] = CoronaQueueHandle;



    request req = { 0 };
    while (1) {
        if ((retval_Sent_From_Dispatch_Queue = xQueueReceive(DispatchQueueHandle, &req, DISPATCH_QUEUE_TIMEOUT)) == pdPASS) {
            department_id id = req.dep_id;

            printf("dispatcher got request: dep_id: %d, requested vehicles: %u, time to comlete: %u\r\n", req.dep_id, req.requested_vehicles, req.time_to_complete);


            if ((retval_Send_to_dep_list[id] = xQueueSendToBack(department_queue_handles_lists[id], &req, QueueTimeoutList[id])) == pdPASS) {
                printf("request sent to department!\r\n");
                switch (id) {
                case FIRE:
                    printf("Fire request was sent to department!\r\n");
                    break;
                case AMBULANCE:
                    printf("Ambulance request was sent to department!\r\n");
                    break;
                case CORONA:
                    printf("Corona request was sent to department!\r\n");
                    break;
                case POLICE:
                    printf("Police request was sent to department!\r\n");
                    break;
                default:
                    break;


                }
            }
            else {
               printf(" dispatcher sending request to department %d failed. time passed or queue is full!\r\n", (int)id);

            }
        }
        else {
           printf(" dispatcher couldnt get request. receiving request from main queue was failed. time passed or queue is empty!\r\n");

        }
    }
}


// @brief: this function takes request from department queue and send to execute thread
TaskFunction_t departmentTask(void* dep_id) {//argument is speartment_id

    department_id depar_id = (department_id)dep_id;
    BaseType_t retval_SR_thread;
    request req;
    int thread_buffer_list[DEPARTMENTS_NUM] = { POLICE_SR_THREAD_BUFFER_SIZE, FIRE_SR_THREAD_BUFFER_SIZE, AMBULANCE_SR_THREAD_BUFFER_SIZE,CORONA_SR_THREAD_BUFFER_SIZE };
    TaskHandle_t SR_handle_array[MAX_SR_HANDLES] = { 0 };
    uint8_t sr_handle_index = 0;

    while (1)
    {
        sr_handle_index %= MAX_SR_HANDLES;
        if (xQueueReceive(department_queue_handles_lists[depar_id], &req, QueueTimeoutList[depar_id]) == pdPASS)
        {
           printf(" department %d received request\r\n", (int)depar_id);
            switch (depar_id) {
            case FIRE:
               // printf("Got request from fire department sending to SR!\r\n");
                break;
            case AMBULANCE:
              //  printf("Got request from ambulance department sending to SR!\r\n");
                break;
            case POLICE:
              //  printf("Got request from police department sending to SR!\r\n");
                break;
            case CORONA:
              //  printf("Got request from corona department sending to SR!\r\n");
                break;
            }
            if ((retval_SR_thread = xTaskCreate(
                ServiceRoutine,
                (const char*)"ServiceRoutineThread",
                thread_buffer_list[depar_id],
                (void const*)&req, //request to be passed
                department_thread_priority[depar_id],
                (const TaskHandle_t*)&SR_handle_array[sr_handle_index])) == pdPASS) //should i add handles to each of task created?
            {
                printf("New SR was created!\r\n");
                sr_handle_index++;
            }
            else {
                printf("NEW SR failed to be created!\r\n");
            }
        }
        else {
            printf("receive request from %d department queue failed. queue empty or time passed!\r\n", (int)depar_id);

        }
    }
}

TaskFunction_t ServiceRoutine(void* req) {
    request* reques = (request*)req;//
    department_id depar_id = reques->dep_id;
    int glob_semaphore_count_before_1 = uxSemaphoreGetCount(globalSemaphore);

    SemaphoreHandle_t department_semaphore = semaphoreList[depar_id];
    printf("department request %d entered service routine\r\n", depar_id);
    int i = 0;
    printf("requested cars: %d, sempahoreglobal: %d, concurrentsemaphore: %d", reques->requested_vehicles, uxSemaphoreGetCount(globalSemaphore), uxSemaphoreGetCount(department_semaphore));
    while (i < reques->requested_vehicles) {
        printf("global sempahore count: %ld, concurrent semaphore count: %ld", uxSemaphoreGetCount(globalSemaphore), uxSemaphoreGetCount(department_semaphore));
        int glob_semaphore_count_before_2 = uxSemaphoreGetCount(globalSemaphore);
        if (xSemaphoreTake(globalSemaphore, 0) == pdPASS && xSemaphoreTake(semaphoreList[depar_id], 0) == pdPASS) {
            printf("global semaphore and depar semaphore were taken! priority is %d\r\n", uxTaskPriorityGet(NULL));
            vTaskPrioritySet(NULL, uxTaskPriorityGet(NULL) + 1);// increases prioirity by one
            i++;
            int glob_semaphore_count_before_3 = uxSemaphoreGetCount(globalSemaphore);
            totalVehicles--;
            department_list[depar_id].available_num--;
            department_list[depar_id].concurrent_num--;
            /* in case all resources for execution acquired, exectues, releases all semaphores and end task*/
            if (i == reques->requested_vehicles - 1) {
                printf("##################################sending vehicles!\r\n");
                vTaskDelay(pdMS_TO_TICKS(reques->time_to_complete));
                releaseResources(i, depar_id);
                printf("released resources!\r\n");
                int glob_semaphore_count_before_4 = uxSemaphoreGetCount(globalSemaphore);
                vTaskDelete(NULL);
            }
        }
        else {
            /* in deadlock releases all semaphores acquired and then start again*/
            releaseResources(i, depar_id);
            vTaskPrioritySet(NULL, (UBaseType_t)department_thread_priority[depar_id]);
            int glob_semaphore_count_before_5 = uxSemaphoreGetCount(globalSemaphore);
            printf("collision detected, released all semaphores!\r\n");
            i = 0;


        }
    }
}



void releaseResources(int i, department_id depar_id) {
    for (int j = 0; j < i;j++) {
        int glob_semaphore_count_before = uxSemaphoreGetCount(globalSemaphore);
        xSemaphoreGive(globalSemaphore);
        int glob_semahpore_count_after = uxSemaphoreGetCount(globalSemaphore);
        xSemaphoreGive(semaphoreList[depar_id]);
        printf("department semaphore and global were given!\r\n");
        totalVehicles++;
        department_list[depar_id].concurrent_num++;
        department_list[depar_id].available_num++;
        printf("released resources of %d department: totalVehicles: %d, concurrent num: %d, availble num: %d\r\n", depar_id, totalVehicles, department_list[depar_id].concurrent_num, department_list[depar_id].available_num);
        //to add delay time to break symmetry?
    }
}






/*

TaskFunction_t timerCallback(void* argument) {
    printf("Dispatcher initial size: %d\r\n", INIT_DISPATCHER_SIZE);
    printf("Dispatcher free size: %d\r\n", dispatch_data.available_num);
    printf("Dispatcher occupied size: %d\r\n", INIT_DISPATCHER_SIZE - dispatch_data.available_num);



}

TaskFunction_t loggingStartThread(void* argument)
{

    TimerHandle_t log_timer = xTimerCreate("LogTimer", pdMS_TO_TICKS(LOG_TIMER_INTERVAL), pdTRUE, 0, timerCallback);
    if (log_timer != NULL) {
        // Start the timer
        xTimerStart(log_timer, 0);
    }

}


*/
TaskFunction_t generateTask(void* argument)
{
    BaseType_t retval_Send_To_Dispatch_Queue = 0;
    while (1) {
        department_id dep = rand() % DEPARTMENTS_NUM;
        uint32_t active_time = 1;//rand() % XTICKS_MAX;
        uint8_t vehicle_num_to_dispatch = (rand() % MAX_NUM_REQUESTED_VEHICLES) + 1; // request num of vehicles can be only greater than zero
        request req = { dep,vehicle_num_to_dispatch,active_time };
        if ((retval_Send_To_Dispatch_Queue = xQueueSendToBack(DispatchQueueHandle, &req, DISPATCH_QUEUE_TIMEOUT)) == pdPASS) {
            printf("generated request. request was put inside main queue! request: dep: %d, active time: %u, vehicle num: %u\r\n", dep, active_time, vehicle_num_to_dispatch);

        }
        else {
            printf("request couldnt enter main queue due to it is full or time passed!\r\n");
        }


    }
}

int __io_putchar(int ch) {
  HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, HAL_MAX_DELAY); // Replace huartX with your UART handle
  return ch;
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */