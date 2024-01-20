#include "hw_main.h"
#include "main.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//extern UART_HandleTypeDef huart4;
//extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart3;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;


unsigned int get_console_message(uint8_t* buff, uint32_t buff_size);

#define UART_DEBUG &huart3
//#define UART_4 &huart4
//#define UART_5 &huart5
#define SPI_1_MASTER	(&hspi1)
#define SPI_2_SLAVE		(&hspi2)
#define I2C_1 &hi2c1
#define I2C_2 &hi2c2
#define BUFF_SIZE 64 // 1024
#define SHORT_TIMEOUT 100
#define PACKET_SIZE 1


uint8_t uart3_flag_callback=0;
uint8_t uart4_flag_callback=0;
uint8_t uart5_flag_callback=0;
uint8_t flag_1=0;
uint8_t flag_2=0;
uint8_t message_buff[BUFF_SIZE]="AbraKadaAra";
uint8_t buff1[BUFF_SIZE]={'\0'};
uint8_t buff2[BUFF_SIZE]={'\0'};

uint8_t msg[BUFF_SIZE]={'\0'};


uint16_t Size=0;

//void ex1()
//{
//	HAL_StatusTypeDef status;
//
//	status = HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"Received message: ", strlen("Received message: "), HAL_MAX_DELAY);
//	if(status != HAL_OK)
//	{
//		printf("UART_Transmit failed with status: %d", status);
//		return;
//	}
//	status = HAL_UART_Transmit(UART_DEBUG, message_buff, strlen((char*) message_buff), HAL_MAX_DELAY);
//	if(status != HAL_OK)
//	{
//		printf("UART_Transmit failed with status: %d", status);
//		return;
//	}
//	status = HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);
//	if(status != HAL_OK)
//	{
//		printf("UART_Transmit failed with status: %d", status);
//		return;
//	}
//	status = HAL_UARTEx_ReceiveToIdle_DMA(UART_5, buff2, BUFF_SIZE)	;
//	if(status != HAL_OK)
//	{
//		printf("UART_5 Receive failed with status: %d", status);
//		return;
//	}
//	status= HAL_UART_Transmit(UART_4 ,message_buff, strlen((char*) message_buff), HAL_MAX_DELAY);
//	if(status != HAL_OK)
//	{
//		printf("UART_4 Transmit failed with status: %d", status);
//		return;
//	}
//
//
//	while(1){
//		if(uart5_flag_callback)
//		{
//			uart5_flag_callback = 0;
//			status = HAL_UARTEx_ReceiveToIdle_DMA(UART_4, buff1, BUFF_SIZE);
//			if(status != HAL_OK)
//			{
//				printf("UART_4 Receive failed with status: %d", status);
//			}
//
//			status = HAL_UART_Transmit(UART_5, buff2, strlen((char*) buff2),HAL_MAX_DELAY);
//			if(status != HAL_OK)
//			{
//				printf("UART_5 TRANSMIT failed with status: %d", status);
//			}
//
//		}
//		if(uart4_flag_callback)
//		{
//			HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"Returned message: ",strlen("Returned message: "),HAL_MAX_DELAY);
//			HAL_UART_Transmit(UART_DEBUG, buff1, BUFF_SIZE,HAL_MAX_DELAY);
//			HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"\r\n",strlen("\r\n"),HAL_MAX_DELAY);
//
//			uart4_flag_callback	=0;
//		}
//	}
//}

void ex2(){
	HAL_StatusTypeDef status;
	unsigned int message_len = get_console_message(msg, BUFF_SIZE);
//	unsigned int message_len = 2;
//	msg[0] = 0xF0;
//	msg[1] = 0xF;

	int i = 0;
	flag_2 = 0;
	status = HAL_SPI_Receive_IT(SPI_2_SLAVE, buff2 + i, message_len);
	if (status != HAL_OK) {
		printf("Failed to setup receive S\n");
	}
	status = HAL_SPI_Transmit(SPI_1_MASTER, msg + i, message_len, HAL_MAX_DELAY);
	if (status != HAL_OK) {
		printf("Failed to transmit data M->S\n");
	}
	while(!flag_2);
	flag_2 = 0;
	flag_1 = 0;
	printf("Slave got message %s\n", buff2);
	uint8_t tmp[BUFF_SIZE] = { '\0' };
	status = HAL_SPI_TransmitReceive_IT(SPI_2_SLAVE, buff2 + i, tmp + i, message_len);
	if (status != HAL_OK) {
		printf("Failed to transmit data S->M\n");
	}
	status = HAL_SPI_Receive(SPI_1_MASTER, buff1 + i, message_len, HAL_MAX_DELAY);
	if (status != HAL_OK) {
		printf("Failed to setup receive M\n");
	}
	while(!flag_1);
	flag_1 = 0;
	printf("Master got message %s\n", buff1);
}

void ex3(){
	HAL_StatusTypeDef status;

	unsigned int message_len=	get_console_message(msg, BUFF_SIZE);
	HAL_I2C_Master_Transmit_DMA(I2C_1, (uint16_t)0, msg, message_len);

	HAL_I2C_Slave_Receive_DMA(I2C_2, buff2,BUFF_SIZE);

	while(1){
		if (flag_2){
			status=HAL_I2C_Master_Transmit_DMA(I2C_2, (uint16_t)0 , buff2 , strlen(buff2));
			status=HAL_I2C_Slave_Receive_DMA(I2C_1, buff1, BUFF_SIZE);

			flag_2=0;
		}

		if (flag_1){
			HAL_UART_Transmit(UART_DEBUG, (const char*)"Returned message: ", strlen("Returned message: "), HAL_MAX_DELAY);

			HAL_UART_Transmit(UART_DEBUG, buff1, strlen(buff1), HAL_MAX_DELAY);
			HAL_UART_Transmit(UART_DEBUG, (const char*)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);

			flag_1=0;
			break;
		}

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


void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c){


		flag_1=1;

}




void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c){
	if (hi2c== I2C_2){
		flag_1=1;
	}
}


void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	if (hspi == SPI_2_SLAVE){
		flag_2=1;
	}
	else if (hspi == SPI_1_MASTER){
		flag_1=1;
	}
}

void hw_main(){
	ex2();
}
