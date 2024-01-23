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
#include "FreeRTOS.h"
#include "task.h"
#include "stdlib.h"
#include "semphr.h"

#include "timers.h"

typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define DEPARTMENTS_NUM 4
#define MAX_NUM_REQUESTED_VEHICLES 5

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
#define LOG_TIMER_INTERVAL 1000
#define DISPATCH_QUEUE_TIMEOUT portMAX_DELAY
#define POLICE_QUEUE_TIMEOUT portMAX_DELAY
#define FIRE_QUEUE_TIMEOUT portMAX_DELAY
#define AMBULANCE_QUEUE_TIMEOUT portMAX_DELAY
#define CORONA_QUEUE_TIMEOUT portMAX_DELAY
#define LOG_TIMER_INTERVAL 1000
#define XTICKS_MAX 5
#define LOGGING_THREAD_PRIORITY (osPriorityNormal)
#define LOG_THREAD_BUFFER_SIZE (128)

/* Define for GenerateThread */
#define GENERATE_THREAD_PRIORITY (osPriorityNormal)
#define GENERATE_THREAD_BUFFER_SIZE (128)

/* Define for DispatchThread */
#define DISPATCH_THREAD_PRIORITY (osPriorityNormal)
#define DISPATCH_THREAD_BUFFER_SIZE (128)

/* Define for PoliceThread */
#define POLICE_THREAD_PRIORITY (osPriorityNormal)
#define POLICE_THREAD_BUFFER_SIZE (128)

/* Define for FireThread */
#define FIRE_THREAD_PRIORITY (osPriorityNormal)
#define FIRE_THREAD_BUFFER_SIZE (128)

/* Define for AmbulanceThread */
#define AMBULANCE_THREAD_PRIORITY (osPriorityNormal)
#define AMBULANCE_THREAD_BUFFER_SIZE (128)

/* Define for CoronaThread */
#define CORONA_THREAD_PRIORITY (osPriorityNormal)
#define CORONA_THREAD_BUFFER_SIZE (128)

#define AMBULANCE_SR_THREAD_PRIORITY (osPriorityNormal)
#define AMBULANCE_SR_THREAD_BUFFER_SIZE (128)

#define POLICE_SR_THREAD_PRIOIRTY (osPriorityNormal)
#define POLICE_SR_THREAD_BUFFER_SIZE (128)

#define CORONA_SR_THREAD_PRIOIRTY (osPriorityNormal)
#define CORONA_SR_THREAD_BUFFER_SIZE (128)

#define FIRE_SR_THREAD_PRIOIRTY (osPriorityNormal)
#define FIRE_SR_THREAD_BUFFER_SIZE (128)

#define SEMAPHORE_WAIT_TIME portMAX_DELAY

//TaskFunction_t loggingStartThread(void *argument);

TaskFunction_t timerCallback(void *argument);
TaskFunction_t ServiceRoutine(void* req);

TaskFunction_t departmentTask(void *argument);

TaskFunction_t dispatchTask(void *argument);
TaskFunction_t generateTask(void *argument);



uint32_t totalVehicles=MAX_NUM_REQUESTED_VEHICLES;
uint32_t coronaConNum=CORONA_CONCURRENT_NUM;
uint32_t policeConNum=POLICE_CONCURRENT_NUM;
uint32_t fireConNum=FIRE_CONCURRENT_NUM;
uint32_t ambulanceConNum=AMBULANCE_CONCURRENT_NUM;

osPriority_t department_thread_priority[DEPARTMENTS_NUM]={POLICE_THREAD_PRIORITY, FIRE_THREAD_PRIORITY, AMBULANCE_THREAD_PRIORITY, CORONA_THREAD_PRIORITY};
typedef enum {
	POLICE,
	FIRE,
	AMBULANCE,
	CORONA
}department_id;
void releaseResources(int i,department_id depar_id);

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
department_data police_dep = {INIT_POLICE_NUM, POLICE_CONCURRENT_NUM, POLICE_CONCURRENT_NUM};
department_data fire_dep = {INIT_FIRE_NUM, FIRE_CONCURRENT_NUM, FIRE_CONCURRENT_NUM};
department_data ambulance_dep = {INIT_AMBULANCE_NUM, AMBULANCE_CONCURRENT_NUM, AMBULANCE_CONCURRENT_NUM};
department_data corona_dep = {INIT_CORONA_NUM, CORONA_CONCURRENT_NUM, CORONA_CONCURRENT_NUM};
department_data department_list[DEPARTMENTS_NUM]={0};
dispatcher_data dispatch_data = {INIT_DISPATCHER_SIZE};

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
TickType_t QueueTimeoutList[DEPARTMENTS_NUM]={POLICE_QUEUE_TIMEOUT,FIRE_QUEUE_TIMEOUT,AMBULANCE_QUEUE_TIMEOUT,CORONA_QUEUE_TIMEOUT};

TaskFunction_t loggingStartThread(void *argument);
SemaphoreHandle_t semaphoreList[DEPARTMENTS_NUM];
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Initialize(void);
static void MPU_Config(void);
void StartDefaultTask(void *argument);

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
	department_list[POLICE] = police_dep;
		department_list[FIRE] = fire_dep ;
		department_list[AMBULANCE] = ambulance_dep;
		department_list[CORONA] = corona_dep;

	policeSemaphore = xSemaphoreCreateCounting(
	    MIN(POLICE_CONCURRENT_NUM, INIT_POLICE_NUM),
	    MIN(POLICE_CONCURRENT_NUM, INIT_POLICE_NUM)
	);
	fireSemaphore = xSemaphoreCreateCounting(
	    MIN(FIRE_CONCURRENT_NUM, INIT_FIRE_NUM),
	    MIN(FIRE_CONCURRENT_NUM, INIT_FIRE_NUM)
	);
	ambulanceSemaphore = xSemaphoreCreateCounting(
	    MIN(AMBULANCE_CONCURRENT_NUM, INIT_AMBULANCE_NUM),
	    MIN(AMBULANCE_CONCURRENT_NUM, INIT_AMBULANCE_NUM)
	);

	coronaSemaphore = xSemaphoreCreateCounting(
	    MIN(CORONA_CONCURRENT_NUM, INIT_CORONA_NUM),
	    MIN(CORONA_CONCURRENT_NUM, INIT_CORONA_NUM)
	);

	globalSemaphore=xSemaphoreCreateCounting(MAX_NUM_REQUESTED_VEHICLES,
	                                        MAX_NUM_REQUESTED_VEHICLES);

	SemaphoreHandle_t semaphoreList[DEPARTMENTS_NUM]={policeSemaphore,fireSemaphore,ambulanceSemaphore,coronaSemaphore};

	  /* Create the queue(s) */
	  /* creation of DispatchQueue */
	  DispatchQueueHandle = xQueueCreate  (DISPATCH_INIT_BUFF_SIZE_BYTES, sizeof(uint32_t));

	  /* creation of AmbulanceQueue */
	  AmbulanceQueueHandle = xQueueCreate (AMBULANCE_INIT_BUFF_SIZE_BYTES, sizeof(uint32_t));

	  /* creation of PoliceQueue */
	  PoliceQueueHandle = xQueueCreate (POLICE_INIT_BUFF_SIZE_BYTES, sizeof(uint32_t));

	  /* creation of FireQueue */
	  FireQueueHandle = xQueueCreate (FIRE_INIT_BUFF_SIZE_BYTES, sizeof(uint32_t));

	  /* creation of CoronaQueue */
	  CoronaQueueHandle = xQueueCreate (CORONA_INIT_BUFF_SIZE_BYTES, sizeof(uint32_t));


	  /* creation of loggingThread */
	  //BaseType_t log_thread = xTaskCreate(&loggingStartThread,(const char*) "loggingStartThread",(const) LOG_THREAD_BUFFER_SIZE, NULL, (UBaseType_t)LOGGING_THREAD_PRIORITY,(TaskHandle_t * const) &loggingThreadHandle);

	  /* USER CODE BEGIN RTOS_THREADS */
	  BaseType_t generate_thread = xTaskCreate(generateTask,
											  (const char*) "generateThread",
											  (const configSTACK_DEPTH_TYPE) 90,
											  NULL,
											  (UBaseType_t)GENERATE_THREAD_PRIORITY,
											  (TaskHandle_t * const) &generateThreadHandle);
	  BaseType_t dispatch_thread = xTaskCreate(dispatchTask,
											  (const char*) "dispatchThread",
											  (const configSTACK_DEPTH_TYPE) 90,
											  NULL,
											  (UBaseType_t)DISPATCH_THREAD_PRIORITY,
											  (TaskHandle_t * const) &dispatchThreadHandle);

	  BaseType_t police_thread = xTaskCreate(departmentTask,
											  (const char*) "policeThread",
											  (const configSTACK_DEPTH_TYPE) 90 ,
											  (void * const)POLICE,
											  (UBaseType_t)POLICE_THREAD_PRIORITY,
											  (TaskHandle_t * const) &policeThreadHandle);

	  BaseType_t fire_thread = xTaskCreate(	  departmentTask,
											  (const char*) "fireThread",
											  (const configSTACK_DEPTH_TYPE) 90,
											  (void * const)FIRE,
											  (UBaseType_t)FIRE_THREAD_PRIORITY,
											  (TaskHandle_t * const) &fireThreadHandle);
	  BaseType_t ambulnace_thread = xTaskCreate(departmentTask,
											  (const char*) "ambulanceThread",
											  (const configSTACK_DEPTH_TYPE) 90,
											  (void * const)AMBULANCE,
											  (UBaseType_t)AMBULANCE_THREAD_PRIORITY,
											  (TaskHandle_t * const) &ambulanceThreadHandle);
	  BaseType_t corona_thread = xTaskCreate(departmentTask,
											  (const char*) "coronaThread",
											  (const configSTACK_DEPTH_TYPE) 90,
											  (void * const)CORONA,
											  (UBaseType_t)CORONA_THREAD_PRIORITY ,
											  (TaskHandle_t * const) &coronaThreadHandle);



	  vTaskStartScheduler();
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
 // @brief: takes request from dispatcher queue and send to relevant department queue
TaskFunction_t dispatchTask(void *argument){

  BaseType_t retval_Sent_From_Dispatch_Queue=0;
  BaseType_t retval_Police_Send=0;
  BaseType_t retval_Fire_Send=0;
  BaseType_t retval_Corona_Send=0;
  BaseType_t retval_Ambulance_Send=0;

  BaseType_t retval_Send_to_dep_list[DEPARTMENTS_NUM]={retval_Police_Send,retval_Fire_Send, retval_Ambulance_Send,retval_Corona_Send};
  department_queue_handles_lists[POLICE]=PoliceQueueHandle;
  department_queue_handles_lists[FIRE]=FireQueueHandle;
  department_queue_handles_lists[AMBULANCE]=AmbulanceQueueHandle;
  department_queue_handles_lists[CORONA]=CoronaQueueHandle;



	request req;
	while(1){
    if ((retval_Sent_From_Dispatch_Queue= xQueueReceive(DispatchQueueHandle, &req, DISPATCH_QUEUE_TIMEOUT )) == pdPASS){

			department_id id=req.dep_id;

      if ((retval_Send_to_dep_list[id]= xQueueSend(department_queue_handles_lists[id], &req, QueueTimeoutList[id])) == pdPASS ){
        switch (id){
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
    }
  }
}


// @brief: this function takes request from department queue and send to execute thread
TaskFunction_t departmentTask(void *dep_id) //argument is speartment_id
{
    department_id depar_id = *((department_id *) dep_id);
    TaskHandle_t policeSRHandle;
    TaskHandle_t ambulanceSRHandle;

    TaskHandle_t fireSRHandle;

    TaskHandle_t coronaSRHandle;

    BaseType_t retval_SR_thread;
    request police_req;
    request ambulance_req;
    request fire_req;
    request corona_req;
    request departments_req_list[DEPARTMENTS_NUM]={police_req,fire_req,ambulance_req,corona_req};
    int thread_buffer_list[DEPARTMENTS_NUM]={POLICE_SR_THREAD_BUFFER_SIZE, FIRE_SR_THREAD_BUFFER_SIZE, AMBULANCE_SR_THREAD_BUFFER_SIZE,CORONA_SR_THREAD_BUFFER_SIZE};
    TaskHandle_t department_SR_handles[DEPARTMENTS_NUM]={policeSRHandle, fireSRHandle, ambulanceSRHandle, coronaSRHandle };



    while (1)
    {
        if (xQueueReceive(department_queue_handles_lists[depar_id], (void const*)&departments_req_list[depar_id], QueueTimeoutList[depar_id]) == pdPASS)
        {
          switch(depar_id){
            case FIRE:
              printf("Got request from fire department!\r\n");
              break;
            case AMBULANCE:
              printf("Got request from ambulance department!\r\n");
              break;
            case POLICE:
              printf("Got request from police department!\r\n");
              break;
            case CORONA:
              printf("Got request from corona department!\r\n");
              break;
          }
          if ((retval_SR_thread = xTaskCreate(
                    ServiceRoutine,
                    (const char *)"ServiceRoutineThread",
                    thread_buffer_list[depar_id],
                    (void const*)&departments_req_list[depar_id], //request to be passed
                    department_thread_priority[depar_id],
                    (const TaskHandle_t *)&department_SR_handles[depar_id])) == pdPASS)
          {
              printf("New SR!\r\n");
          }
        }
    }
}

TaskFunction_t ServiceRoutine(void* req){
	request* reques = (request*) req ;//
  department_id depar_id=reques->dep_id;
  SemaphoreHandle_t department_semaphore= semaphoreList[depar_id];

	int i=0;
  while (i <reques->requested_vehicles){

    if (xSemaphoreTake(globalSemaphore,0) ==pdPASS && xSemaphoreTake(semaphoreList[depar_id], 0) == pdPASS ){
      vTaskPrioritySet(NULL, uxTaskPriorityGet(NULL) + 1);// increases prioirity by one
      i++;
      totalVehicles--;
      department_list[POLICE].available_num--;
      department_list[POLICE].concurrent_num--;
      /* in case all resources for execution acquired, exectues, releases all semaphores and end task*/
      if (i==reques->requested_vehicles-1){
        vTaskDelay(reques->time_to_complete);
        releaseResources(i, depar_id);
        vTaskDelete(NULL);
      }
    }
    else{
      /* in deadlock releases all semaphores acquired and then start again*/
      releaseResources(i, depar_id);
      i=0;


    }
  }
}



void releaseResources(int i,department_id depar_id){
  for (int j=0; j<i;j++){
      xSemaphoreGive(globalSemaphore);
      xSemaphoreGive(semaphoreList[depar_id]);
      totalVehicles++;
      department_list[POLICE].concurrent_num++;
      department_list[POLICE].available_num++;
      //to add delay time to break symmetry?
  }
}


TaskFunction_t generateTask(void *argument)
{
  BaseType_t retval_Send_To_Dispatch_Queue=0;
	while (1) {
		department_id dep =rand() % DEPARTMENTS_NUM;
		TickType_t active_time= rand() % XTICKS_MAX;
		uint8_t vehicle_num_to_dispatch= rand() % MAX_NUM_REQUESTED_VEHICLES;
		request req= {dep,active_time,vehicle_num_to_dispatch};
		if ((retval_Send_To_Dispatch_Queue= xQueueSend(DispatchQueueHandle, &req, DISPATCH_QUEUE_TIMEOUT )) == pdPASS){
			printf("request was put inside dispatch queue!\r\n");
		}
	}
}





TaskFunction_t timerCallback(void *argument){
	printf("Dispatcher initial size: %d\r\n", INIT_DISPATCHER_SIZE);
	printf("Dispatcher free size: %d\r\n", dispatch_data.available_num);
	printf("Dispatcher occupied size: %d\r\n", INIT_DISPATCHER_SIZE-dispatch_data.available_num);



}

TaskFunction_t loggingStartThread(void *argument)
{

  TimerHandle_t log_timer  = xTimerCreate("LogTimer", pdMS_TO_TICKS(LOG_TIMER_INTERVAL), pdTRUE, 0, timerCallback);
  if (log_timer != NULL) {
    // Start the timer
    xTimerStart(log_timer, 0);
  }

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

/* USER CODE END Header_loggingStartThread */
//TaskFunction_t loggingStartThread(void *argument)
//{
  /* USER CODE BEGIN loggingStartThread */
  /* Infinite loop */
	//osTimerId_t log_timer= osTimerNew(timerCallback, osTimerPeriodic , NULL, NULL);
	//osStatus_t status = osTimerStart(log_timer, LOG_TIME_INTERVAL);

//}

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
