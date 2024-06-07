#include "main.h"
#include "timer_testing.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Testing_Program.h"


extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart3;


#define UART_DEBUG &huart3
#define TIME &htim6
#define RT &hrtc
int flag=0;

void timer_testing(uint8_t* new_interval, uint32_t* return_from_test_value){
	RTC_TimeTypeDef Time;
	int interval_sec=atoi((const char*)new_interval);


	// Update current_interval and recalculate auto-reload value

	htim6.Instance->ARR = (interval_sec*72*10^6/(htim6.Init.Prescaler+1))-1; // SET TIMER ACCORDING TO USER SECONDS INPUT INTERVAL
	 /* Get the current Time */
	HAL_RTC_GetTime(RT, &Time, RTC_FORMAT_BIN);

	  /* Get the current Date */

	  /* Process the date and time values (e.g., print to console, store in variables) */
	printf("first sample current Time: %02d:%02d:%02d\n", Time.Hours, Time.Minutes, Time.Seconds);
	uint32_t first_sample_time=Time.Hours*3600+Time.Minutes*60+Time.Seconds;
	HAL_TIM_Base_Start(TIME);
	while (1){
		if (flag){ //sample time again
			HAL_RTC_GetTime(RT, &Time, RTC_FORMAT_BIN);
			printf("second sample current Time: %02d:%02d:%02d\n", Time.Hours, Time.Minutes, Time.Seconds);

			uint32_t second_sample_time=Time.Hours*3600+Time.Minutes*60+Time.Seconds;
			uint32_t delta=second_sample_time-first_sample_time;
			printf("interval: %d\n",delta);
			flag=0;
			*return_from_test_value=delta;
			break;
		}
	}


}


void TIMx_IRQHandler(void) {

	flag=1;

}
