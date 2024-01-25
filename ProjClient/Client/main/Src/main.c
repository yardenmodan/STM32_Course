#include <stdio.h>
#include "main.h"
#include "string.h"
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "time.h"

#define MAX_MSG_SIZE 256
#define SERVER_PORT 8080
#define CLIENT_PORT 8080
#define CLOCKS_PER_SEC

typedef struct __attribute__((__packed__)) {
    uint32_t TestID;
    uint8_t Peripheral;
    uint8_t iter_num;
    uint8_t pattern_len;
    uint8_t str[MAX_MSG_SIZE];
} packet_from_client;

typedef struct __attribute__((__packed__)) {
    uint8_t TestID[TESTID_LEN];
    uint8_t TestResult;
} packet_to_client;

typedef enum {
    TEST_ID,
    PERIPHERAL,
    ITER_NUM,
    PATTERN_LEN,
    STR
} packet_field;

extern UART_HandleTypeDef huart3;
#define UART_DEBUG &huart3



int main() {
    while (1) {
        /* ############# take inputs from user and generate fields of packet ############## */

        uint8_t Test_ID_str[11] = {0};
        uint8_t message_len_str[4] = {0};
        uint8_t iter_nums_str[4] = {0};
        uint8_t Peripheral_num_str[6] = {0};
        uint8_t str[MAX_MSG_SIZE + 1] = {0};

        uint8_t* field_list[5] = {Test_ID_str, message_len_str, iter_nums_str, Peripheral_num_str, str};
        packet_field iter_field = TEST_ID;
        int iter_str = 0;

        // this while loop receives input from user for each packet field
        while (iter_field != STR) {
            uint8_t* chosen_field = field_list[iter_field];
            getUserInput(iter_field, chosen_field, &iter_str);
        }

        /* ############# translation of strings to packet fields ############## */

        packet_from_client msg_packet = {0};
        translateStringsToPacket(&msg_packet, Test_ID_str, Peripheral_num_str, iter_nums_str, message_len_str, str);

        /* ############# pack all fields into struct ############## */

        char serializedPacket[264] = {0};
        char receivedPacket[5] = {0};
        memcpy(serializedPacket, &msg_packet, sizeof(msg_packet));

        /* ############# send test across ethernet using udp ############## */

        sendTestUDP(serializedPacket);
    }
    return 0;
}



//#include <stdio.h>
//#include "main.h"
//#include "string.h"
//#include <stdlib.h>
//#define MAX_MSG_SIZE 256
//#include <unistd.h>
//#include <arpa/inet.h>
//#include "time.h"
//#define SERVER_PORT 8080
//#define CLIENT_PORT 8080
//#define CLOCKS_PER_SEC
//typedef struct __attribute__((__packed__)) {
//	uint32_t TestID;
//	uint8_t Peripheral;
//	uint8_t iter_num;
//	uint8_t pattern_len;
//	uint8_t str[MAX_MSG_SIZE];
//}packet_from_client;
//
//
//typedef struct __attribute__((__packed__)) {
//	uint8_t TestID[TESTID_LEN];
//	uint8_t TestResult;
//
//}packet_to_client;
//
//typedef enum {
//	TEST_ID,
//	PERIPHERAL,
//	ITER_NUM,
//	PATTERN_LEN,
//	STR
//}packet_field;
//
//extern UART_HandleTypeDef huart3;
//#define UART_DEBUG &huart3
//
//
//int main(){
//	while(1){
//		/*#############take inputs from user and generate fields of packet ##############*/
//
//		uint8_t Test_ID_str[11]=0;
//		uint32_t Test_ID=0;
//
//		uint8_t message_len_str[4]=0;
//		uint8_t message_len=0;
//		uint8_t iter_nums_str[4]=0;
//		uint8_t iter_nums=0;
//		uint8_t Peripheral_num_str[6]=0;
//		uint8_t Peripheral=0;
//		uint8_t	str[MAX_MSG_SIZE+1]=0;
//		uint8_t field_list[5]={Test_ID_str, message_len_str, iter_nums_str, Peripheral_num_str,str};
//		packet_field iter_field=0;
//		int iter_str=0;
//		char *endptr=0;
//
//		/* this while loop receives input from user for each packet field*/
//		while(iter_field!=5){
//
//			chosen_field=field_list[iter_field];
//			if (iter_str==0){ //ready to get a enter value to specific field
//				switch (iter_field){
//				case (TEST_ID):
//					HAL_UART_Transmit(UART_DEBUG, "Please enter Test ID:\r\n", strlen("Please enter Test ID:\r\n"), HAL_MAX_DELAY);
//					break;
//
//				case (PERIPHERAL):
//					HAL_UART_Transmit(UART_DEBUG, "Please enter Peripheral name (Timer, ADC, SPI, I2C, UART):\r\n", strlen("Please enter Peripheral name (Timer, ADC, SPI, I2C, UART):\r\n"), HAL_MAX_DELAY);
//					break;
//
//				case (ITER_NUM):
//					HAL_UART_Transmit(UART_DEBUG, "Please enter number of iterations to run (256 iterations tops):\r\n", strlen("Please enter number of iterations to run (256 iterations tops):\r\n"), HAL_MAX_DELAY);
//					break;
//
//				case (PATTERN_LEN):
//					HAL_UART_Transmit(UART_DEBUG, "Please enter string length (256 characters tops):\r\n", strlen("Please enter string length (256 character tops):\r\n"), HAL_MAX_DELAY);
//					break;
//
//				case (STR):
//					HAL_UART_Transmit(UART_DEBUG, "Please enter a message (256 characters tops):\r\n", strlen("Please enter a message (256 character tops):\r\n"), HAL_MAX_DELAY);
//					break;
//				default:
//					break;
//				}
//			}
//			HAL_UART_Receive(UART_DEBUG, chosen_field+iter_str, 1, HAL_MAX_DELAY);
//			if (Test_ID_str[iter_str]=='\r'){// user pressed enter
//				iter_field++;
//				iter_str=0;
//				continue;
//			}
//			iter_str++;
//
//
//		}
//		/*#############translation of strings to packet fields ##############*/
//
//		/* */
//		uint32_t Test_ID=(uint32_t)strtol(Test_ID_str, &endptr,10); //add error handling!
//		switch (Peripheral_num_str){
//		case ("ADC"):
//			Peripheral=16;
//			break;
//		case ("Timer"):
//			Peripheral=1;
//			break;
//		case ("I2C"):
//			Peripheral=8;
//			break;
//		case ("SPI"):
//			Peripheral=4;
//			break;
//		case ("UART"):
//			Peripheral=2;
//			break;
//		default:
//			break;
//		}
//
//		uint8_t Iter_num=(uint8_t)strtol(iter_nums_str, &endptr,10); //add error handling!
//		uint8_t message_len=(uint8_t)strtol(message_len_str, &endptr,10); //add error handling!
//
//		/*#############pack all fields into struct ##############*/
//
//		packet_from_client msg_packet={0};
//		msg_packet.Peripheral=Peripheral;
//		msg_packet.TestID=Test_ID;
//		msg_packet.iter_num=iter_nums;
//		msg_packet.pattern_len=message_len;
//		strncpy(msg_packet.str, str, 257);
//		/*should serialize struct to buffer before sending*/
//		char serializedPacket[264]={0};
//		char receivedPacket[5]={0};
//		memcpy(serializedPacket, &msg_packet, sizeof(msg_packet));
//
//		/*#############send test across ethernet using udp ##############*/
//
//		/*  need to send via udp*/
//		int sockfd;
//		struct sockaddr_in client_addr,server_addr;
//
//		// Create a socket for UDP communication
//		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
//			perror("Socket creation failed");
//			exit(EXIT_FAILURE);
//		}
//
//			// Configure server address
//		memset(&client_addr, 0, sizeof(client_addr));
//		client_addr.sin_family = AF_INET;
//		client_addr.sin_port = htons(CLIENT_PORT);
//		client_addr.sin_addr.s_addr = INADDR_ANY;
//		memset(&server_addr,0,sizeof(server_addr));
//		server_addr.sin_familty=AF_INET;
//		if (inet_pton(AF_INET, "192.168.007.002", &(server_addr.sin_addr))<=0){
//			perror("Invalid IP address\r\n");
//			exit(EXIT_FAILURE);
//		}
//		server_addr.sin_port=htons(SERVER_PORT);
//
//		// Example message to be sent
//		if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1) {
//			perror("Bind failed\r\n");
//			close(sockfd);
//			exit(EXIT_FAILURE);
//		}
//		clock_t start_time = clock();
//		time_t current_tine=time(NULL);
//		struct tm *local_time=localtime(&current_time);
//		char time_str[64];
//		strftime(time_str,sizeof(time_str),"%Y-%m-%d %H:%M:%S", local_time);
//
//		if (sendto(sockfd, (const char*)serializedPacket, strlen(serializedPacket), 0, (struct sockaddr *)&server_addr, sizeof(server_addr))){
//			perror("Sendto failed\r\n");
//			close(sockfd);
//			exit(EXIT_FAILURE);
//		}
//
//		printf("UDP packet send successfully!\r\n");
//		size_t bytes_received=0;
//		if ( bytes_received=recfrom(sockfd,receivedPacket,sizeof(receivedPacket),0, (struct sockaddr *)&server_addr, sizeof(server_addr))){
//			perror("Receiving failed!\r\n");
//			close(sockfd);
//			exit(EXIT_FAILURE);
//		}
//		// Send the message to the server
//		clock_t end_time= clock();
//		double elapsed_time=((double)(end_time-start_time)/CLOCKS_PER_SEC);
//		printf("UDP packet received successfully!\r\n");
//		packet_to_client received_packet={0};
//		memcpy(received_packet.TestID,receivedPacket,4);
//		memcpy(received_packet.TestResult, receivedPacket[4],1);
//		char test_result[10]={0};
//		(received_packet.Test==0x01U)? strcpy(test_results, "Success", 8): strcpy(test_results, "Failure", 8) ;
//		/*#############take input and puts in file ##############*/
//
//		FILE *file;
//		if (file=fopen("Test_Results.txt", "a")==NULL){
//			perror("Error opening the file!");
//			return 1;
//		}
//
//		fprintf(file,"Date: [%s], Time Taken: %.6f seconds, Test ID: %d, Result: %s \r\n",time_str, elapsed_time,received_packet.TestID, test_result);
//
//		close(file);
//		// Close the socket
//		close(sockfd);
//	}
//}
