#include "main.h"
#include "uart.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Testing_Program.h"


extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart2;
#define UART_2 &huart2
#define UART_5 &huart5

uint16_t Size=0;
int uart2_flag_callback=0;
int uart5_flag_callback=0;

void uart_testing(uint8_t * str_in, uint8_t* return_from_test_value)
{
	HAL_StatusTypeDef status;
	uint8_t buff_5[BUFF_SIZE]={0};
	uint8_t buff_2[BUFF_SIZE]={0};
	status = HAL_UARTEx_ReceiveToIdle_DMA(UART_5, buff_5, Size);
	if(status != HAL_OK)
	{
		printf("UART_5 Transmit failed with status: %d", status);
		return;
	}
	status= HAL_UART_Transmit(UART_2 ,str_in, strlen(str_in), HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_2 Transmit failed with status: %d", status);
		return;
	}


	while(1){
		if(uart5_flag_callback)
		{

			uart5_flag_callback = 0;
			status= HAL_UARTEx_ReceiveToIdle_DMA(UART_2, return_from_test_value, Size);
			if(status != HAL_OK)
			{
				printf("UART_2 Receive failed with status: %d", status);
				return;
			}
			status = HAL_UART_Transmit(UART_5, buff_5, strlen(buff_5) ,HAL_MAX_DELAY);
			if(status != HAL_OK)
			{
				printf("UART_5 TRANSMIT failed with status: %d", status);
				return;
			}

		}
		if(uart2_flag_callback)
		{
			uart2_flag_callback=0;
			return;
		}

	}


}



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {



	if (huart==UART_5){
		uart5_flag_callback=1;
	}

	if (huart==UART_2){
		uart2_flag_callback=1;

	}
}


