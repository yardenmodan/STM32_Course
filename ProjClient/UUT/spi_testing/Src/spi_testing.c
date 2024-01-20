#include "main.h"
#include "spi_testing.h"
#include "Testing_Program.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

#define SPI_MASTER &hspi1
#define SPI_SLAVE &hspi2

int spi_master_callback=0;
int spi_slave_callback=0;
void spi_testing(uint8_t * str_in, uint8_t* return_from_test_value)
{

	uint8_t buff[BUFF_SIZE]={0};
	status= HAL_SPI_Receive_DMA( SPI_SLAVE ,buff_5 , msg_size);
	if(status != HAL_OK)
	{
		printf("SPI Receive failed with status: %d", status);
		return;
	}
	status= HAL_SPI_Transmit(SPI_MASTER, str_in, msg_size, HAL_MAX_DELAY);
	{
		printf("SPI MASTER Transmit failed with status: %d", status);
		return;
	}


	while(1){
		if(spi_slave_callback)
		{

			spi_slave_callback = 0;
			status = HAL_SPI_Receive_DMA(SPI_MASTER, return_from_test_value, msg_size);
			if(status != HAL_OK)
			{
				printf("SPI_MASTER Receive failed with status: %d", status);
			}

			status = HAL_SPI_Transmit(SPI_SLAVE, buff_5, msg_size ,HAL_MAX_DELAY);
			if(status != HAL_OK)
			{
				printf("SPI_SLAVE TRANSMIT failed with status: %d", status);
			}

		}
		if(spi_master_callback)
		{
			return return_from_test_value;

		}

	}


}

void HAL_SPI_RxCpltCallback(UART_HandleTypeDef *hspi)

{


	if (hspi==SPI_MASTER){
		spi_master_callback=1;
	}

	if (hspi==SPI_SLAVE){
		spi_slave_callback=1;

	}

}
