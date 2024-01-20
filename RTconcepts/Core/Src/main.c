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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
//typedef StaticSemaphore_t osStaticSemaphoreDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define XTICKS_MAX 5
#define DEPARTMENTS_NUM 4
#define MAX_NUM_REQUESTED_VEHICLES 5
#define XTICKS_DELAY_DISPATCH_QUEUE 20
#define XTICKS_DELAY_DEPARTMENT_QUEUE 20
#define INIT_POLICE_NUM 10
#define INIT_AMBULANCE_NUM 10
#define INIT_FIRE_NUM 10
#define INIT_CORONA_NUM 10
#define CORONA_CONCURRENT_NUM 4
#define AMBULANCE_CONCURRENT_NUM 4
#define POLICE_CONCURRENT_NUM 3
#define FIRE_CONCURRENT_NUM 2
#define INIT_DISPATCHER_SIZE 50
#define DISPATCH_INIT_BUFF_SIZE_BYTES 1024
#define AMBULANCE_INIT_BUFF_SIZE_BYTES 1024
#define FIRE_INIT_BUFF_SIZE_BYTES 1024
#define CORONA_INIT_BUFF_SIZE_BYTES 1024
#define POLICE_INIT_BUFF_SIZE_BYTES 1024
#define LOG_TIME_INTERVAL 1000
#define DISPATCH_QUEUE_PRIORITY osPriorityHigh
#define DISPATCH_QUEUE_TIMEOUT 0
#define LOGGING_THREAD_PRIORITY (osPriorityNormal)
#define LOG_THREAD_BUFFER (128)

/* Define for GenerateThread */
#define GENERATE_THREAD_PRIORITY (osPriorityNormal)
#define GENERATE_THREAD_BUFFER (128)

/* Define for DispatchThread */
#define DISPATCH_THREAD_PRIORITY (osPriorityNormal)
#define DISPATCH_THREAD_BUFFER (128)

/* Define for PoliceThread */
#define POLICE_THREAD_PRIORITY (osPriorityNormal)
#define POLICE_THREAD_BUFFER (128)

/* Define for FireThread */
#define FIRE_THREAD_PRIORITY (osPriorityNormal)
#define FIRE_THREAD_BUFFER (128)

/* Define for AmbulanceThread */
#define AMBULANCE_THREAD_PRIORITY (osPriorityNormal)
#define AMBULANCE_THREAD_BUFFER (128)

/* Define for CoronaThread */
#define CORONA_THREAD_PRIORITY (osPriorityNormal)
#define CORONA_THREAD_BUFFER (128)


/* USER CODE END PD */



void timerCallback(void *argument);
void ServiceRoutineCorona(void* corona_req);
void ServiceRoutineAmbulance(void* ambulance_req);
void ServiceRoutineFire(void* fire_req);
void ServiceRoutinePolice(void* police_req1);
void coronaTask(void *argument);
void fireTask(void *argument);
void ambulanceTask(void *argument);
void policeTask(void *argument);
void dispatchTask(void *argument);
void generateTask(void *argument);

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
	uint32_t totalVehicles=MAX_NUM_REQUESTED_VEHICLES;
	uint32_t coronaConNum=CORONA_CONCURRENT_NUM;
	uint32_t policeConNum=POLICE_CONCURRENT_NUM;
	uint32_t fireConNum=FIRE_CONCURRENT_NUM;
	uint32_t ambulanceConNum=AMBULANCE_CONCURRENT_NUM;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
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
	TickType_t time_to_complete;
}request;

typedef struct {
	uint8_t available_num;
}dispatcher_data;

/* Definitions for defaultTask
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};*/
department_data police_dep = {INIT_POLICE_NUM, POLICE_CONCURRENT_NUM, POLICE_CONCURRENT_NUM};
department_data fire_dep = {INIT_FIRE_NUM, FIRE_CONCURRENT_NUM, FIRE_CONCURRENT_NUM};
department_data ambulance_dep = {INIT_AMBULANCE_NUM, AMBULANCE_CONCURRENT_NUM, AMBULANCE_CONCURRENT_NUM};
department_data corona_dep = {INIT_CORONA_NUM, CORONA_CONCURRENT_NUM, CORONA_CONCURRENT_NUM};
department_data department_list[DEPARTMENTS_NUM]={0};
dispatcher_data dispatch_data = {INIT_DISPATCHER_SIZE};

osMutexId_t policeMutex;
osMutexId_t globalMutex;

osMutexId_t ambulanceMutex;
osMutexId_t fireMutex;
osMutexId_t coronaMutex;

/* Definitions for loggingThread */
osThreadId_t loggingThreadHandle;
uint32_t loggingThreadBuffer[LOG_THREAD_BUFFER];
osStaticThreadDef_t loggingThreadControlBlock;
const osThreadAttr_t loggingThread_attributes = {
  .name = "loggingThread",
  .cb_mem = &loggingThreadControlBlock,
  .cb_size = sizeof(loggingThreadControlBlock),
  .stack_mem = &loggingThreadBuffer[0],
  .stack_size = sizeof(loggingThreadBuffer),
  .priority = (osPriority_t)LOGGING_THREAD_PRIORITY,
};

/* Definitions for GenerateThread */
osThreadId_t generateThreadHandle;
uint32_t generateThreadBuffer[GENERATE_THREAD_BUFFER];
osStaticThreadDef_t generateThreadControlBlock;
const osThreadAttr_t generateThread_attributes = {
  .name = "generateThread",
  .cb_mem = &generateThreadControlBlock,
  .cb_size = sizeof(generateThreadControlBlock),
  .stack_mem = &generateThreadBuffer[0],
  .stack_size = sizeof(generateThreadBuffer),
  .priority = (osPriority_t)GENERATE_THREAD_PRIORITY,
};

/* Definitions for DispatchThread */
osThreadId_t dispatchThreadHandle;
uint32_t dispatchThreadBuffer[DISPATCH_THREAD_BUFFER];
osStaticThreadDef_t dispatchThreadControlBlock;
const osThreadAttr_t dispatchThread_attributes = {
  .name = "dispatchThread",
  .cb_mem = &dispatchThreadControlBlock,
  .cb_size = sizeof(dispatchThreadControlBlock),
  .stack_mem = &dispatchThreadBuffer[0],
  .stack_size = sizeof(dispatchThreadBuffer),
  .priority = (osPriority_t)DISPATCH_THREAD_PRIORITY,
};

/* Definitions for PoliceThread */
osThreadId_t policeThreadHandle;
uint32_t policeThreadBuffer[POLICE_THREAD_BUFFER];
osStaticThreadDef_t policeThreadControlBlock;
const osThreadAttr_t policeThread_attributes = {
  .name = "policeThread",
  .cb_mem = &policeThreadControlBlock,
  .cb_size = sizeof(policeThreadControlBlock),
  .stack_mem = &policeThreadBuffer[0],
  .stack_size = sizeof(policeThreadBuffer),
  .priority = (osPriority_t)POLICE_THREAD_PRIORITY,
};

/* Definitions for FireThread */
osThreadId_t fireThreadHandle;
uint32_t fireThreadBuffer[FIRE_THREAD_BUFFER];
osStaticThreadDef_t fireThreadControlBlock;
const osThreadAttr_t fireThread_attributes = {
  .name = "fireThread",
  .cb_mem = &fireThreadControlBlock,
  .cb_size = sizeof(fireThreadControlBlock),
  .stack_mem = &fireThreadBuffer[0],
  .stack_size = sizeof(fireThreadBuffer),
  .priority = (osPriority_t)FIRE_THREAD_PRIORITY,
};

/* Definitions for AmbulanceThread */
osThreadId_t ambulanceThreadHandle;
uint32_t ambulanceThreadBuffer[AMBULANCE_THREAD_BUFFER];
osStaticThreadDef_t ambulanceThreadControlBlock;
const osThreadAttr_t ambulanceThread_attributes = {
  .name = "ambulanceThread",
  .cb_mem = &ambulanceThreadControlBlock,
  .cb_size = sizeof(ambulanceThreadControlBlock),
  .stack_mem = &ambulanceThreadBuffer[0],
  .stack_size = sizeof(ambulanceThreadBuffer),
  .priority = (osPriority_t)AMBULANCE_THREAD_PRIORITY,
};

/* Definitions for CoronaThread */
osThreadId_t coronaThreadHandle;
uint32_t coronaThreadBuffer[CORONA_THREAD_BUFFER];
osStaticThreadDef_t coronaThreadControlBlock;
const osThreadAttr_t coronaThread_attributes = {
  .name = "coronaThread",
  .cb_mem = &coronaThreadControlBlock,
  .cb_size = sizeof(coronaThreadControlBlock),
  .stack_mem = &coronaThreadBuffer[0],
  .stack_size = sizeof(coronaThreadBuffer),
  .priority = (osPriority_t)CORONA_THREAD_PRIORITY,
};
/* Definitions for DispatchQueue */
osMessageQueueId_t DispatchQueueHandle;
uint8_t DispatchQueueBuffer[ DISPATCH_INIT_BUFF_SIZE_BYTES * sizeof( uint32_t ) ];
osStaticMessageQDef_t DispatchQueueControlBlock;
const osMessageQueueAttr_t DispatchQueue_attributes = {
  .name = "DispatchQueue",
  .cb_mem = &DispatchQueueControlBlock,
  .cb_size = sizeof(DispatchQueueControlBlock),
  .mq_mem = &DispatchQueueBuffer,
  .mq_size = sizeof(DispatchQueueBuffer)
};
/* Definitions for AmbulanceQueue */
osMessageQueueId_t AmbulanceQueueHandle;
uint8_t AmbulanceQueueBuffer[ AMBULANCE_INIT_BUFF_SIZE_BYTES * sizeof( uint32_t ) ];
osStaticMessageQDef_t AmbulanceQueueControlBlock;
const osMessageQueueAttr_t AmbulanceQueue_attributes = {
  .name = "AmbulanceQueue",
  .cb_mem = &AmbulanceQueueControlBlock,
  .cb_size = sizeof(AmbulanceQueueControlBlock),
  .mq_mem = &AmbulanceQueueBuffer,
  .mq_size = sizeof(AmbulanceQueueBuffer)
};
/* Definitions for PoliceQueue */
osMessageQueueId_t PoliceQueueHandle;
uint8_t PoliceQueueBuffer[ POLICE_INIT_BUFF_SIZE_BYTES * sizeof( uint32_t ) ];
osStaticMessageQDef_t PoliceQueueControlBlock;
const osMessageQueueAttr_t PoliceQueue_attributes = {
  .name = "PoliceQueue",
  .cb_mem = &PoliceQueueControlBlock,
  .cb_size = sizeof(PoliceQueueControlBlock),
  .mq_mem = &PoliceQueueBuffer,
  .mq_size = sizeof(PoliceQueueBuffer)
};
/* Definitions for FireQueue */
osMessageQueueId_t FireQueueHandle;
uint8_t FireQueueBuffer[ FIRE_INIT_BUFF_SIZE_BYTES * sizeof( uint32_t ) ];
osStaticMessageQDef_t FireQueueControlBlock;
const osMessageQueueAttr_t FireQueue_attributes = {
  .name = "FireQueue",
  .cb_mem = &FireQueueControlBlock,
  .cb_size = sizeof(FireQueueControlBlock),
  .mq_mem = &FireQueueBuffer,
  .mq_size = sizeof(FireQueueBuffer)
};
/* Definitions for CoronaQueue */
osMessageQueueId_t CoronaQueueHandle;
uint8_t CoronaQueueBuffer[ CORONA_INIT_BUFF_SIZE_BYTES * sizeof( uint32_t ) ];
osStaticMessageQDef_t CoronaQueueControlBlock;
const osMessageQueueAttr_t CoronaQueue_attributes = {
  .name = "CoronaQueue",
  .cb_mem = &CoronaQueueControlBlock,
  .cb_size = sizeof(CoronaQueueControlBlock),
  .mq_mem = &CoronaQueueBuffer,
  .mq_size = sizeof(CoronaQueueBuffer)
};
/* Definitions for AmbulanceSemaphore */
//osSemaphoreId_t AmbulanceSemaphoreHandle;
//const osSemaphoreAttr_t AmbulanceSemaphore_attributes = {
//  .name = "AmbulanceSemaphore"
//};
///* Definitions for PoliceSemaphore */
//osSemaphoreId_t PoliceSemaphoreHandle;
//const osSemaphoreAttr_t PoliceSemaphore_attributes = {
//  .name = "PoliceSemaphore"
//};
///* Definitions for AmbulanceSemaphoreStatic */
//osSemaphoreId_t AmbulanceSemaphoreStaticHandle;
//osStaticSemaphoreDef_t Ambulance_SCB;
//const osSemaphoreAttr_t AmbulanceSemaphoreStatic_attributes = {
//  .name = "AmbulanceSemaphoreStatic",
//  .cb_mem = &Ambulance_SCB,
//  .cb_size = sizeof(Ambulance_SCB),
//};
///* Definitions for PoliceSemaphoreStatic */
//osSemaphoreId_t PoliceSemaphoreStaticHandle;
//osStaticSemaphoreDef_t Police_SCB;
//const osSemaphoreAttr_t PoliceSemaphoreStatic_attributes = {
//  .name = "PoliceSemaphoreStatic",
//  .cb_mem = &Police_SCB,
//  .cb_size = sizeof(Police_SCB),
//};
///* Definitions for FireSemaphoreStatic */
//osSemaphoreId_t FireSemaphoreStaticHandle;
//osStaticSemaphoreDef_t Fire_SCB;
//const osSemaphoreAttr_t FireSemaphoreStatic_attributes = {
//  .name = "FireSemaphoreStatic",
//  .cb_mem = &Fire_SCB,
//  .cb_size = sizeof(Fire_SCB),
//};
///* Definitions for CoronaSemaphoreStatic */
//osSemaphoreId_t CoronaSemaphoreStaticHandle;
//osStaticSemaphoreDef_t Fire_SCB_new;
//const osSemaphoreAttr_t CoronaSemaphoreStatic_attributes = {
//  .name = "CoronaSemaphoreStatic",
//  .cb_mem = &Fire_SCB_new,
//  .cb_size = sizeof(Fire_SCB_new),
//};
///* Definitions for TotalSemaphoreDynamic */
//osSemaphoreId_t TotalSemaphoreDynamicHandle;
//const osSemaphoreAttr_t TotalSemaphoreDynamic_attributes = {
//  .name = "TotalSemaphoreDynamic"
//};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Initialize(void);
static void MPU_Config(void);
void StartDefaultTask(void *argument);
void loggingStartThread(void *argument);

/* USER CODE BEGIN PFP */

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
	department_list[0] = police_dep;
	department_list[1] = fire_dep ;
	department_list[2] = ambulance_dep;
	department_list[3] = corona_dep;

	policeMutex=osMutexNew(NULL);
	globalMutex=osMutexNew(NULL);

	ambulanceMutex=osMutexNew(NULL);
	fireMutex=osMutexNew(NULL);
	coronaMutex=osMutexNew(NULL);

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */


  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
//  /* creation of AmbulanceSemaphore */
//  AmbulanceSemaphoreHandle = osSemaphoreNew(4, 4, &AmbulanceSemaphore_attributes);
//
//  /* creation of PoliceSemaphore */
//  PoliceSemaphoreHandle = osSemaphoreNew(3, 3, &PoliceSemaphore_attributes);
//
//  /* creation of AmbulanceSemaphoreStatic */
//  AmbulanceSemaphoreStaticHandle = osSemaphoreNew(4, 4, &AmbulanceSemaphoreStatic_attributes);
//
//  /* creation of PoliceSemaphoreStatic */
//  PoliceSemaphoreStaticHandle = osSemaphoreNew(3, 3, &PoliceSemaphoreStatic_attributes);
//
//  /* creation of FireSemaphoreStatic */
//  FireSemaphoreStaticHandle = osSemaphoreNew(2, 2, &FireSemaphoreStatic_attributes);
//
//  /* creation of CoronaSemaphoreStatic */
//  CoronaSemaphoreStaticHandle = osSemaphoreNew(4, 4, &CoronaSemaphoreStatic_attributes);
//
//  /* creation of TotalSemaphoreDynamic */
//  TotalSemaphoreDynamicHandle = osSemaphoreNew(50, 50, &TotalSemaphoreDynamic_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of DispatchQueue */
  DispatchQueueHandle = osMessageQueueNew (DISPATCH_INIT_BUFF_SIZE_BYTES, sizeof(uint32_t), &DispatchQueue_attributes);

  /* creation of AmbulanceQueue */
  AmbulanceQueueHandle = osMessageQueueNew (AMBULANCE_INIT_BUFF_SIZE_BYTES, sizeof(uint32_t), &AmbulanceQueue_attributes);

  /* creation of PoliceQueue */
  PoliceQueueHandle = osMessageQueueNew (POLICE_INIT_BUFF_SIZE_BYTES, sizeof(uint32_t), &PoliceQueue_attributes);

  /* creation of FireQueue */
  FireQueueHandle = osMessageQueueNew (FIRE_INIT_BUFF_SIZE_BYTES, sizeof(uint32_t), &FireQueue_attributes);

  /* creation of CoronaQueue */
  CoronaQueueHandle = osMessageQueueNew (CORONA_INIT_BUFF_SIZE_BYTES, sizeof(uint32_t), &CoronaQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
 // defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of loggingThread */
  loggingThreadHandle = osThreadNew(loggingStartThread, NULL, &loggingThread_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  generateThreadHandle = osThreadNew(generateTask, NULL, &generateThread_attributes);
  dispatchThreadHandle = osThreadNew(dispatchTask, NULL, &dispatchThread_attributes);

  policeThreadHandle = osThreadNew(policeTask, NULL, &policeThread_attributes);

  fireThreadHandle = osThreadNew(fireTask, NULL, &fireThread_attributes);
  ambulanceThreadHandle = osThreadNew(ambulanceTask, NULL, &ambulanceThread_attributes);
  coronaThreadHandle = osThreadNew(coronaTask, NULL, &coronaThread_attributes);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
void generateTask(void *argument)
{
	while (1){
		department_id dep =rand() % DEPARTMENTS_NUM;
		TickType_t active_time= rand() % XTICKS_MAX;
		uint8_t vehicle_num_to_dispatch= rand() % MAX_NUM_REQUESTED_VEHICLES;
		request req= {dep,active_time,vehicle_num_to_dispatch};
		if (osMessageQueuePut(DispatchQueueHandle, &req, DISPATCH_QUEUE_PRIORITY, DISPATCH_QUEUE_TIMEOUT )==osOK){
			printf("request was put inside dispatch queue!\r\n");
		}
	}
}
  /* USER CODE BEGIN 5 */
  /* Infinite loop */

void dispatchTask(void *argument){


	request req;
	while(1){
		if (osMessageQueueGet(DispatchQueueHandle, (void*)&req, 0, 0) == osOK){
			switch(req.dep_id){
			case (POLICE):
				osMessageQueuePut(PoliceQueueHandle, &req, 0, 0);
				printf("Police request was sent to department!\r\n");
				break;

			case (FIRE):
				osMessageQueuePut(FireQueueHandle, &req, 0, 0);
				printf("Fire request was sent to department!\r\n");

				break;
			case (AMBULANCE):
				osMessageQueuePut(AmbulanceQueueHandle, &req, 0, 0);
				printf("Ambulance request was sent to department!\r\n");

				break;
			case (CORONA):
				osMessageQueuePut(CoronaQueueHandle, &req, 0, 0);
				printf("Corona request was sent to department!\r\n");

				break;
			default:
				break;

			}
			osDelay(XTICKS_DELAY_DEPARTMENT_QUEUE);
		}
	}
}


void policeTask(void *argument)
{


	request police_req;
	while (1){
		if (osMessageQueueGet(PoliceQueueHandle, &police_req, 0, osWaitForever) == osOK){

			osThreadNew(ServiceRoutinePolice, (void*)&police_req, NULL);


		}

	}

}
void ambulanceTask(void *argument)
{


	request ambulance_req;
	while (1){
		if (osMessageQueueGet(AmbulanceQueueHandle, &ambulance_req, 0, osWaitForever) == osOK){

			osThreadNew(ServiceRoutineAmbulance, (void* )&ambulance_req, NULL);


		}

	}

}
void fireTask(void *argument)
{


	request fire_req;
	while (1){
		if (osMessageQueueGet(FireQueueHandle, &fire_req, 0, osWaitForever) == osOK){

			osThreadNew(ServiceRoutineFire, (void* )&fire_req, NULL);


		}

	}

}
void coronaTask(void *argument)
{


	request corona_req;
	while (1){
		if (osMessageQueueGet(CoronaQueueHandle, &corona_req, 0, osWaitForever) == osOK){

			osThreadNew(ServiceRoutineCorona, (void* )&corona_req, NULL);


		}

	}

}



void ServiceRoutinePolice(void* argument){
	request* police_req = (request*) argument ;//
	int i=0;
	while (i<police_req->requested_vehicles){
		osMutexAcquire(globalMutex,osWaitForever);
		osMutexAcquire(policeMutex, osWaitForever);

		if (department_list[POLICE].available_num>0 && totalVehicles>0 && department_list[POLICE].concurrent_num>0){
			i++;

			/*if (osSemaphoreAcquire(PoliceSemaphoreStaticHandle, osWaitForever)==osOK){
			if (osSemaphoreAcquire(TotalSemaphoreDynamicHandle, osWaitForever)==osOK){*/
			totalVehicles--;
			osMutexRelease(globalMutex);
			department_list[POLICE].available_num--;
			department_list[POLICE].concurrent_num--;
			osMutexRelease(policeMutex);
			if (i==police_req->requested_vehicles-1){
				osDelay(police_req->time_to_complete);
				//osSemaphoreRelease(PoliceSemaphoreStaticHandle);
				//osSemaphoreRelease(TotalSemaphoreDynamicHandle);
				osMutexAcquire(policeMutex, osWaitForever);
				osMutexAcquire(globalMutex, osWaitForever);

				totalVehicles++;
				osMutexRelease(globalMutex);
				department_list[POLICE].concurrent_num++;
				department_list[POLICE].available_num++;
				osMutexRelease(policeMutex);


			}
		}
		else{
			osMutexRelease(globalMutex);
			osMutexRelease(policeMutex);
		}
	}
}


void ServiceRoutineFire(void* argument){
	request * fire_req = (request*) argument ;//
	int i=0;
	while (i<fire_req->requested_vehicles){
		osMutexAcquire(globalMutex,osWaitForever);
		osMutexAcquire(fireMutex, osWaitForever);

		if (department_list[FIRE].available_num>0 && totalVehicles>0 && department_list[FIRE].concurrent_num>0){
			i++;

			/*if (osSemaphoreAcquire(PoliceSemaphoreStaticHandle, osWaitForever)==osOK){
			if (osSemaphoreAcquire(TotalSemaphoreDynamicHandle, osWaitForever)==osOK){*/
			totalVehicles--;
			osMutexRelease(globalMutex);
			department_list[FIRE].available_num--;
			department_list[FIRE].concurrent_num--;
			osMutexRelease(fireMutex);
			if (i==fire_req->requested_vehicles-1){
				osDelay(fire_req->time_to_complete);
				//osSemaphoreRelease(PoliceSemaphoreStaticHandle);
				//osSemaphoreRelease(TotalSemaphoreDynamicHandle);
				osMutexAcquire(fireMutex, osWaitForever);
				osMutexAcquire(globalMutex, osWaitForever);

				totalVehicles++;
				osMutexRelease(globalMutex);
				department_list[FIRE].concurrent_num++;
				department_list[FIRE].available_num++;
				osMutexRelease(fireMutex);


			}
		}
		else{
			osMutexRelease(globalMutex);
			osMutexRelease(fireMutex);
		}
	}
}

void ServiceRoutineAmbulance(void* argument){
	request* ambulance_req = (request*) argument ;//
	int i=0;
	while (i<ambulance_req->requested_vehicles){
		osMutexAcquire(globalMutex,osWaitForever);
		osMutexAcquire(ambulanceMutex, osWaitForever);

		if (department_list[AMBULANCE].available_num>0 && totalVehicles>0 && department_list[AMBULANCE].concurrent_num>0){
			i++;

			/*if (osSemaphoreAcquire(PoliceSemaphoreStaticHandle, osWaitForever)==osOK){
			if (osSemaphoreAcquire(TotalSemaphoreDynamicHandle, osWaitForever)==osOK){*/
			totalVehicles--;
			osMutexRelease(globalMutex);
			department_list[AMBULANCE].available_num--;
			department_list[AMBULANCE].concurrent_num--;
			osMutexRelease(ambulanceMutex);
			if (i==ambulance_req->requested_vehicles-1){
				osDelay(ambulance_req->time_to_complete);
				//osSemaphoreRelease(PoliceSemaphoreStaticHandle);
				//osSemaphoreRelease(TotalSemaphoreDynamicHandle);
				osMutexAcquire(ambulanceMutex, osWaitForever);
				osMutexAcquire(globalMutex, osWaitForever);

				totalVehicles++;
				osMutexRelease(globalMutex);
				department_list[AMBULANCE].concurrent_num++;
				department_list[AMBULANCE].available_num++;
				osMutexRelease(ambulanceMutex);


			}
		}
		else{
			osMutexRelease(globalMutex);
			osMutexRelease(ambulanceMutex);
		}
	}
}

void ServiceRoutineCorona(void* argument){
	request* corona_req = (request*) argument ;//
	int i=0;
	while (i<corona_req->requested_vehicles){
		osMutexAcquire(globalMutex,osWaitForever);
		osMutexAcquire(coronaMutex, osWaitForever);

		if (department_list[CORONA].available_num>0 && totalVehicles>0 && department_list[CORONA].concurrent_num>0){
			i++;

			/*if (osSemaphoreAcquire(PoliceSemaphoreStaticHandle, osWaitForever)==osOK){
			if (osSemaphoreAcquire(TotalSemaphoreDynamicHandle, osWaitForever)==osOK){*/
			totalVehicles--;
			osMutexRelease(globalMutex);
			department_list[CORONA].available_num--;
			department_list[CORONA].concurrent_num--;
			osMutexRelease(coronaMutex);
			if (i==corona_req->requested_vehicles-1){
				osDelay(corona_req->time_to_complete);
				//osSemaphoreRelease(PoliceSemaphoreStaticHandle);
				//osSemaphoreRelease(TotalSemaphoreDynamicHandle);
				osMutexAcquire(coronaMutex, osWaitForever);
				osMutexAcquire(globalMutex, osWaitForever);

				totalVehicles++;
				osMutexRelease(globalMutex);
				department_list[CORONA].concurrent_num++;
				department_list[CORONA].available_num++;
				osMutexRelease(coronaMutex);


			}
		}
		else{
			osMutexRelease(globalMutex);
			osMutexRelease(coronaMutex);
		}
	}
}

  /* USER CODE END 5 */

void timerCallback(void *argument){
	printf("Dispatcher initial size: %d\r\n", INIT_DISPATCHER_SIZE);
	printf("Dispatcher free size: %d\r\n", dispatch_data.available_num);
	printf("Dispatcher occupied size: %d\r\n", INIT_DISPATCHER_SIZE-dispatch_data.available_num);



}
/* USER CODE BEGIN Header_loggingStartThread */
/**
* @brief Function implementing the loggingThread thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_loggingStartThread */
void loggingStartThread(void *argument)
{
  /* USER CODE BEGIN loggingStartThread */
  /* Infinite loop */
	osTimerId_t log_timer= osTimerNew(timerCallback, osTimerPeriodic , NULL, NULL);
	osStatus_t status = osTimerStart(log_timer, LOG_TIME_INTERVAL);

}

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
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