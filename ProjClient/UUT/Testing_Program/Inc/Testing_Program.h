

#ifndef INC_TESTING_PROGRAM_H_
#define INC_TESTING_PROGRAM_H_

#include "udp.h"
#include "ip_addr.h"
#include "err.h"
#include "arch.h"

#define MAX_MSG_SIZE 257
#define TESTID_LEN 4
#define TRUE 0x01
#define FALSE 0xFF
// Define the UART debug port
extern UART_HandleTypeDef huart3;
#define UART_DEBUG &huart3

// Define the port number for the server

typedef enum{
	TIMER=1,
	UART=2,
	SPI=4,
	I2C=8,
	ADC_P=16
}Peripheral;

typedef struct __attribute__((packed)) {
	uint32_t TestID;
	Peripheral Peripheral;
	uint8_t iter_num;
	uint8_t pattern_len;
	uint8_t str[MAX_MSG_SIZE];
}packet_from_client;


typedef struct __attribute__((packed)) {
	uint32_t TestID[TESTID_LEN];
	uint8_t TestResult;

}packet_to_client;
// Define the lwIP network interface









// Define function prototypes
                  // Initialize the UDP server
void Testing_Program(void);

#endif
