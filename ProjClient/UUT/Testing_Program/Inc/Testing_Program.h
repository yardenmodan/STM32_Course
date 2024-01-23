/*
 *
 *
 *  Created on:
 *      Author:
 *
 *  This header file defines global constants, variables, and functions used in the RTG program.
 */

#ifndef INC_TESTING_PROGRAM_H_
#define INC_TESTING_PROGRAM_H_

#include "main.h"
#include "inet.h"
#include "lwip.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_MSG_SIZE 256
#define MAX_INCOMING_BUFF_SIZE= MAX_MSG_SIZE+7
#define TESTID_LEN 4
#define TRUE 0x01
#define FALSE 0xFF
// Define the UART debug port
extern UART_HandleTypeDef huart3;
#define UART_DEBUG &huart3

// Define the port number for the server


typedef struct  {
	uint32_t TestID;
	uint8_t Peripheral;
	uint8_t iter_num;
	uint8_t pattern_len;
	uint8_t str[MAX_MSG_SIZE];
}packet_from_client;


typedef struct  {
	uint8_t TestID[TESTID_LEN];
	uint8_t TestResult;

}packet_to_client;
// Define the lwIP network interface
extern struct netif gnetif;	//Generic data structure used for all lwIP network interfaces.

// Define constants for special characters
#define END_OF_STRING 0
#define BACKSPACE 8
#define LINE_FEED 10
#define CARRIAGE_RETURN 13

// Define the maximum length of the incoming packet buffer

// Define global variables
extern uint8_t callback_flag;        			// Flag used to signal when a packet has been received
extern uint8_t incoming_buffer[MAX_MSG_SIZE+7]; 	// Incoming packet buffer
extern struct udp_pcb* upcb;         			// UDP protocol control block
extern ip_addr_t dest_ipaddr;        			// Destination IP address
extern u16_t dest_port;           			// Destination port number
extern u16_t incoming_len;
struct udp_pcb;                      			// Forward declaration of the udp_pcb structure

// Define function prototypes
void udpServer_init();                  // Initialize the UDP server


#endif /* INC_RTG_H_ */
