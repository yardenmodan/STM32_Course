
#include "main.h"
#include "adc_testing.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "Testing_Program.h"

extern ADC_HandleTypeDef hadc1;
extern DAC_HandleTypeDef hdac;

#define ADC_1 &hadc1
#define DAC_1 &hdac
#define PACKET_SIZE 1
#define DEC_BASE 10


int adc_flag=0;

void adc_testing(uint8_t * str_in, uint8_t* return_from_test_value) {


	uint32_t num= atoi((const char*)str_in);
	HAL_DAC_SetValue(DAC_1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,(uint32_t) num);
	HAL_DAC_Start(DAC_1, DAC_CHANNEL_1);

	while(1){


		if (adc_flag) {
			adc_flag=0;
			uint32_t adcValue = HAL_ADC_GetValue(ADC_1);
			itoa(adcValue, (char*)return_from_test_value, DEC_BASE);
			return;


		}

	}
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   // Make sure it's the correct ADC instance

    adc_flag=1;
    // Process ADC data (adcValue)

}

