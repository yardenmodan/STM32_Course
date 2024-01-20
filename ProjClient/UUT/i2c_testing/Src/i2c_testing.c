#include "main.h"

extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c4;

#define I2C_MASTER &hi2c2
#define I2C_SLAVE &hi2c4

uint8_t* i2c_testing(uint8_t * str_in,  uint16_t msg_size, uint8_t* return_from_test_value)
{
	int i2c_slave_callback=0;
	int i2c_master_callback=0;

	uint8_t buff_5[BUFF_SIZE]={0};
	status= HAL_I2C_Slave_Receive_IT(I2C_SLAVE, buff_5, msg_size);
	if(status != HAL_OK)
	{
		printf("I2C_SLAVE Receive failed with status: %d", status);
		return;
	}
	status= HAL_I2C_Master_Transmit(I2C_MASTER, 0, str, msg_size, HAL_MAX_DELAY);
	if(status != HAL_OK)
	{
		printf("I2C_MASTER Transmit failed with status: %d", status);
		return;
	}


	while(1){
		if(i2c_slave_callback)
		{

			i2c_slave_callback = 0;
			status= HAL_I2C_Slave_Receive_DMA(I2C_MASTER,return_from_test_value , msg_size);
			if(status != HAL_OK)
			{
				printf("I2C_MASTER Receive failed with status: %d", status);
				return;
			}

			status = HAL_I2C_Transmit(I2C_SLAVE,0, buff_5, msg_size ,HAL_MAX_DELAY);
			if(status != HAL_OK)
			{
				printf("I2C SLAVE failed with status: %d", status);
			}

		}
		if(i2c_master_callback)
		{
			return return_from_test_value;

		}

	}


}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)

{


	if (huart==I2C_SLAVE){
		i2c_master_callback=1;
	}

	if (huart==I2C_MASTER){
		i2c_slave_callback=1;

	}

}
