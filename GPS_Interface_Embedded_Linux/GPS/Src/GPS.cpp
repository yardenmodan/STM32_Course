#include <iostream>
#include "main.h"
#include <stm32f7xx_hal_rng.h>
#include <stm32f7xx_hal_crc.h>
#include <stm32f7xx_hal_uart.h>
#include <vector>


extern CRC_HandleTypeDef hcrc;
extern RNG_HandleTypeDef hrng;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
HAL_StatusTypeDef status;


#define ID_LEN 20
#define CRC_LEN
#define X_COOR_LEN
#define Y_COOR_LEN
#define UART1 &huart1
#define UART_DEBUG &huart3
#define CRCH &hcrc
#define HRNG &hrng

int __io_putchar(int ch) {
  uint8_t c = ch;
  HAL_UART_Transmit(UART_DEBUG, &c, 1, HAL_MAX_DELAY); // Assumes huart2 is the UART handle
  return ch;
}


typedef enum {
    ID_FIRST,
    X_COORDINATE,
    Y_COORDINATE,
	ID_LAST,
    CRC_F,
	FIELD_NUMBER
} field_iter;

class GPS{
private:

	field_iter iter;
	uint32_t ID;
	uint8_t req; // gps generation request
	uint32_t input[FIELD_NUMBER];
	std::vector<uint8_t> output;


public:
	GPS(){
		this->iter=ID_FIRST;
		this->ID=0;
		this->req=0;

	}
	// rule of 5
	void receive_signal(){
		status=HAL_UART_Receive(UART1, &req, 1, HAL_MAX_DELAY);
		if (status!=HAL_OK){
			printf("Receiving was unsuccessful!\r\n");
			return;
		}
		req=1;
	}

	void insertID(){
		input[ID_FIRST]=ID;

		input[ID_LAST]=ID;
		ID++;


	}

	void generate_coordinates(){
		status= HAL_RNG_GenerateRandomNumber(HRNG, input+X_COORDINATE);
		if (status!=HAL_OK){
			printf("Generation of random number was unsuccessful!\r\n");
			return;
		}
		status= HAL_RNG_GenerateRandomNumber(HRNG, input+Y_COORDINATE);
		if (status!=HAL_OK){
			printf("Generation of random number was unsuccessful!\r\n");
			return;
		}

	}

	void addCRC(){
		input[CRC_F]= HAL_CRC_Calculate(CRCH,input,FIELD_NUMBER);



	}
	void packetify(){
		output.resize(4*FIELD_NUMBER);
		for (auto i=0; i<FIELD_NUMBER;i++){
			output[i*4+0]=input[i]>>24&0xff;
			output[i*4+1]=input[i]>>16&0xff;
			output[i*4+2]=input[i]>>8&0xff;
			output[i*4+3]=input[i]&0xff;
		}
		printf("packetify completed!\r\n");


	}




	void transmit_packet(){
		status=HAL_UART_Transmit(UART1, output.data() ,output.size(),HAL_MAX_DELAY );
		if (status!=HAL_OK){
			printf("Transmission was unsuccessful!\r\n");
		}
		req=0;
		printf("transmission completed successfully with %d characters transmitted!\r\n", output.size());

	}



};



void GPS1() {
    GPS gps; // default constructor
    gps.receive_signal();
    gps.insertID();
    gps.generate_coordinates();
    gps.addCRC();
    gps.packetify();
    gps.transmit_packet();
}
