#include "ex_uart.h"
#include "main.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

unsigned int get_console_message(uint8_t* buff, uint32_t buff_size);

#define UART_DEBUG &huart3
#define UART_4 &huart4
#define UART_5 &huart5
#define BUFF_SIZE 1024
#define SHORT_TIMEOUT 100
#define PACKET_SIZE 1


uint8_t uart3_flag_callback=0;
uint8_t uart4_flag_callback=0;
uint8_t uart5_flag_callback=0;
uint8_t ex1_buff4[BUFF_SIZE]="AbraKadaAra";
uint8_t ex1_buff5[BUFF_SIZE]={0};
uint8_t ex2_buff4[BUFF_SIZE]={0};
uint8_t ex2_buff5[BUFF_SIZE]={0};
uint8_t ex4_buff3[BUFF_SIZE]={0};
uint16_t Size=0;




void ex1(void){
	int i=0;
	HAL_StatusTypeDef status;
	unsigned int msg_length;
	status = HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"Received message: ", strlen("Received message: "), HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_Transmit failed with status: %d", status);
		return;
	}
	status = HAL_UART_Transmit(UART_DEBUG, ex1_buff4, BUFF_SIZE, HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_Transmit failed with status: %d", status);
		return;
	}
	status = HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_Transmit failed with status: %d", status);
		return;
	}
	while(1){

		status = HAL_UART_Transmit(UART_4, ex1_buff4+i, PACKET_SIZE, HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_Transmit failed with status: %d", status);
			return;
		}

		status = HAL_UART_Receive(UART_5, ex1_buff5+i, PACKET_SIZE, HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_Receive failed with status: %d", status);
			return;
		}
		if (ex1_buff5[i]=='\0' || i>= BUFF_SIZE-1){
			msg_length =i+1;
			i=0;
			break;
		}
		i++;
	}
	memset(ex1_buff4, 0 , BUFF_SIZE);


	while (i<msg_length){
		status = HAL_UART_Transmit(UART_5, ex1_buff5+i,PACKET_SIZE, HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_Transmit failed with status: %d", status);
			return;
		}

		status = HAL_UART_Receive(UART_4, ex1_buff4+i, PACKET_SIZE, HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_Transmit failed with status: %d", status);
			return;
		}
		i++;
	}
	HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"Returned message: ",strlen("Returned message: "),HAL_MAX_DELAY);
	HAL_UART_Transmit(UART_DEBUG, ex1_buff4,msg_length,HAL_MAX_DELAY);
	HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"\r\n",strlen("\r\n"),HAL_MAX_DELAY);

	memset(ex1_buff5, 0 , BUFF_SIZE);



}


void ex2()
{
	int i=0;
	HAL_StatusTypeDef status;
	unsigned int message_length = get_console_message(ex2_buff4, BUFF_SIZE);

	while (i<message_length){
		status = HAL_UART_Transmit(UART_4, ex2_buff4+i, PACKET_SIZE, HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_4_Transmit failed with status: %d", status);
			return;
		}

		status = HAL_UART_Receive(UART_5, ex2_buff5+i,PACKET_SIZE, HAL_MAX_DELAY);// there is a problem here
		if(status != HAL_OK)
		{
			printf("UART_5_Receive failed with status: %d", status);
			return;
		}
		i++;
	}
	memset(ex2_buff4, 0 , BUFF_SIZE);
	i=0;
	while (i<message_length){
		status = HAL_UART_Transmit(UART_5, ex2_buff5+i, PACKET_SIZE, HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_5_Transmit failed with status: %d", status);
			return;
		}

		status = HAL_UART_Receive(UART_4, ex2_buff4+i, PACKET_SIZE, HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_4_Receive failed with status: %d", status);
			return;
		}
		i++;
	}
	HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"Returned message: ",strlen("Returned message: "),HAL_MAX_DELAY);
	HAL_UART_Transmit(UART_DEBUG, ex2_buff4,message_length,HAL_MAX_DELAY);
	HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"\r\n",strlen("\r\n"),HAL_MAX_DELAY);
	memset(ex2_buff5, 0 , BUFF_SIZE);



}







void ex3()
{
	HAL_StatusTypeDef status;

	status = HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"Received message: ", strlen("Received message: "), HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_Transmit failed with status: %d", status);
		return;
	}
	status = HAL_UART_Transmit(UART_DEBUG, ex1_buff4, BUFF_SIZE, HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_Transmit failed with status: %d", status);
		return;
	}
	status = HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_Transmit failed with status: %d", status);
		return;
	}
	status = HAL_UARTEx_ReceiveToIdle_IT(UART_5, ex1_buff5, BUFF_SIZE)	;
	if(status != HAL_OK)
	{
		printf("UART_5 Receive failed with status: %d", status);
		return;
	}
	status= HAL_UART_Transmit(UART_4 ,ex1_buff4, strlen(ex1_buff4), HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_4 Transmit failed with status: %d", status);
		return;
	}
	memset(ex1_buff4, 0 , BUFF_SIZE);

	while(1){
		if(uart5_flag_callback)
		{
			uart5_flag_callback = 0;
			status = HAL_UARTEx_ReceiveToIdle_IT(UART_4, ex1_buff4, BUFF_SIZE);
			if(status != HAL_OK)
			{
				printf("UART_4 Receive failed with status: %d", status);
			}

			status = HAL_UART_Transmit(UART_5, ex1_buff5, strlen(ex1_buff5),HAL_MAX_DELAY);
			if(status != HAL_OK)
			{
				printf("UART_5 TRANSMIT failed with status: %d", status);
			}


		}
		if(uart4_flag_callback)
		{
			HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"Returned message: ",strlen("Returned message: "),HAL_MAX_DELAY);
			HAL_UART_Transmit(UART_DEBUG, ex1_buff4, BUFF_SIZE,HAL_MAX_DELAY);
			HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"\r\n",strlen("\r\n"),HAL_MAX_DELAY);

			uart4_flag_callback	=0;


		}

	}


}

void ex4()

{
	HAL_StatusTypeDef status;
	status = HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"\r\n", 2,HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_DEBUG Transmit failed with status: %d", status);
	}
	unsigned int message_length = get_console_message(ex4_buff3, (uint32_t)BUFF_SIZE);
	status= HAL_UARTEx_ReceiveToIdle_IT(UART_5, ex2_buff5, (uint16_t)BUFF_SIZE);
	if(status != HAL_OK)
	{
		printf("UART_DEBUG Receive failed with status: %d", status);
	}
	status= HAL_UART_Transmit(UART_4,ex4_buff3, message_length, HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_4 Transmit failed with status: %d", status);
	}

	while(1){

		if(uart4_flag_callback)
		{
			memset(ex2_buff5 ,0,BUFF_SIZE);


			uart4_flag_callback = 0;
			status = HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"Returned message: ", strlen("Returned message: "),HAL_MAX_DELAY);
			if(status != HAL_OK)
			{
				printf("UART_DEBUG Transmit failed with status: %d", status);
			}
			status = HAL_UART_Transmit(UART_DEBUG, ex2_buff4, strlen(ex2_buff4),HAL_MAX_DELAY);
			if(status != HAL_OK)
			{
				printf("UART_DEBUG Transmit failed with status: %d", status);
			}

			memset(ex2_buff4 ,0,BUFF_SIZE);
//
			break;

		}
		if(uart5_flag_callback)
		{
			memset(ex4_buff3,0,BUFF_SIZE);

			uart5_flag_callback = 0;
			status = HAL_UARTEx_ReceiveToIdle_IT(UART_4, ex2_buff4, BUFF_SIZE);
			if(status != HAL_OK)
			{
				printf("UART_4 Receive failed with status: %d", status);
			}

			status = HAL_UART_Transmit(UART_5, ex2_buff5, strlen(ex2_buff5), HAL_MAX_DELAY);
			if(status != HAL_OK)
			{
				printf("UART_5 TRANSMIT failed with status: %d", status);
			}


		}
	}
}
//
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if (huart==UART_DEBUG){
		uart3_flag_callback=1;
	}

	if (huart==UART_5){
		uart5_flag_callback=1;
	}

	if (huart==UART_4){
		uart4_flag_callback=1;

	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {



	if (huart==UART_5){
		uart5_flag_callback=1;
	}

	if (huart==UART_4){
		uart4_flag_callback=1;

	}
}


unsigned int get_console_message(uint8_t* buff, uint32_t buff_size){
	HAL_StatusTypeDef status;
	int i=0;
	unsigned int message_len=0;
	status= HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"Please enter a message (1024 character maximum):\r\nReceived message: ", strlen("Please enter a message (1024 character maximum):\r\nReceived message: "),HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_Transmit failed with status: %d", status);
		return 0;
	}
	while(1){
		status=HAL_UART_Receive(UART_DEBUG, (uint8_t*) buff+i , PACKET_SIZE, HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_Receive failed with status: %d", status);
			return 0;
		}
		if (buff[i]=='\r' || i>= BUFF_SIZE-1){
			status= HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"\n\r", strlen("\n\r"),HAL_MAX_DELAY);
			if(status != HAL_OK)
			{
				printf("UART_Transmit failed with status: %d", status);
				return 0;
			}
			message_len=i+1;
			i=0;
			break;
		}
		status= HAL_UART_Transmit(UART_DEBUG, (uint8_t*)buff+i,PACKET_SIZE,HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_Transmit failed with status: %d", status);
			return 0;
		}
		i++;

	}
	return message_len;


}
void hw_main() {
	ex4();
	// ex2();
}

