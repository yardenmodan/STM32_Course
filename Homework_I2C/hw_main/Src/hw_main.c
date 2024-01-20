#include "main.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hw_main.h"

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart3;

#define I2C_1 &hi2c1
#define I2C_2 &hi2c2
#define UART_DEBUG &huart3
#define BUFF_SIZE 1024
#define PACKET_SIZE 1

int flag_1=0;
int flag_2=0;
HAL_StatusTypeDef status1;
HAL_StatusTypeDef status2;
void ex1(){
	uint8_t msg[BUFF_SIZE]="AbraKadabra";
	uint8_t i2c1_buff[BUFF_SIZE]={0};

	uint8_t i2c2_buff[BUFF_SIZE]={0};
	HAL_UART_Transmit(UART_DEBUG, (const char*)"Received message: ", strlen("Received message: "), HAL_MAX_DELAY);

	HAL_UART_Transmit(UART_DEBUG, msg, (uint16_t)strlen(msg), HAL_MAX_DELAY);
	HAL_UART_Transmit(UART_DEBUG, (const char*)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);
	status1= HAL_I2C_Master_Transmit_IT(I2C_1, (uint16_t)0, msg, (uint16_t)strlen(msg));
	status2= HAL_I2C_Slave_Receive(I2C_2, i2c2_buff, BUFF_SIZE, HAL_MAX_DELAY);

	while(1){
		if (flag_2){
			status1= HAL_I2C_Master_Transmit_IT(I2C_2, (uint16_t)0, i2c2_buff, (uint16_t)strlen(i2c2_buff));
			status2= HAL_I2C_Slave_Receive(I2C_1, i2c1_buff, BUFF_SIZE, HAL_MAX_DELAY);
			flag_2=0;
		}
		if (flag_1){
			HAL_UART_Transmit(UART_DEBUG, (const char*)"Returned message: ", strlen("Returned message: "), HAL_MAX_DELAY);

			HAL_UART_Transmit(UART_DEBUG, i2c1_buff, (uint16_t)strlen(i2c1_buff), HAL_MAX_DELAY);
			HAL_UART_Transmit(UART_DEBUG, (const char*)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);

			flag_1=0;
		}
	}
}

void ex2(){
	uint8_t msg[BUFF_SIZE]={0};
	uint8_t i2c2_buff[BUFF_SIZE]={0};
	uint8_t i2c1_buff[BUFF_SIZE]={0};
	unsigned int message_len=	get_console_message(msg, BUFF_SIZE);
	HAL_I2C_Master_Transmit_IT(I2C_1, (uint16_t)0, msg, message_len);
	HAL_I2C_Slave_Receive(I2C_2, i2c2_buff, BUFF_SIZE, HAL_MAX_DELAY);


	while(1){
		if (flag_2){
			status2= HAL_I2C_Master_Transmit_IT(I2C_2, (uint16_t)0 , i2c2_buff , strlen(i2c2_buff));

			status1= HAL_I2C_Slave_Receive(I2C_1, i2c1_buff, BUFF_SIZE,HAL_MAX_DELAY);
			flag_2=0;
		}
		if (flag_1){
			HAL_UART_Transmit(UART_DEBUG, (const char*)"Returned message: ", strlen("Returned message: "), HAL_MAX_DELAY);

			HAL_UART_Transmit(UART_DEBUG, i2c1_buff, strlen(i2c1_buff), HAL_MAX_DELAY);
			HAL_UART_Transmit(UART_DEBUG, (const char*)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);

			flag_1=0;
			break;
		}

	}
}



void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c){
	if (hi2c== I2C_2){
		flag_1=1;
	}
	if (hi2c== I2C_1){
		flag_2=1;
	}
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c){
	if (hi2c== I2C_2){
		flag_2=1;
	}
	if (hi2c== I2C_1){
		flag_1=1;
	}
}
unsigned int get_console_message(uint8_t* buff, uint32_t buff_size){
	HAL_StatusTypeDef status;
	int i=0;
	unsigned int message_len=0;
	status= HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"Please enter a message (1024 character maximum):\r\nReceived message: ", (uint16_t)strlen("Please enter a message (1024 character maximum):\r\nReceived message: "),HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("UART_Transmit failed with status: %d", status);
		return 0;
	}
	while(1){
		status=HAL_UART_Receive(UART_DEBUG, (uint8_t*) buff+i , (uint16_t)PACKET_SIZE, HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_Receive failed with status: %d", status);
			return 0;
		}
		if (buff[i]=='\r' || i>= BUFF_SIZE-1){
			status= HAL_UART_Transmit(UART_DEBUG, (const uint8_t*)"\n\r", (uint16_t)strlen("\n\r"),HAL_MAX_DELAY);
			if(status != HAL_OK)
			{
				printf("UART_Transmit failed with status: %d", status);
				return 0;
			}
			message_len=i+1;
			i=0;
			break;
		}
		status= HAL_UART_Transmit(UART_DEBUG, (uint8_t*)buff+i,(uint16_t)PACKET_SIZE,HAL_MAX_DELAY);
		if(status != HAL_OK)
		{
			printf("UART_Transmit failed with status: %d", status);
			return 0;
		}
		i++;

	}
	return message_len;


}
void hw_main(){
	ex1();
}
