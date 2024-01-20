#include "Testing_Program.h"
#include "Server.h"
#include <string.h>
#include "Parsing.h"
#include "main.h"

#define BUFF_SIZE MAX_MSG_LEN-7
#define TESTID_LEN 4
#define TRUE 0x01
#define FALSE 0xFF

typedef enum{
	TIMER=1,
	UART=2,
	SPI=4,
	I2C=8,
	ADC_p=16

}peripheral_t;



typedef struct packet_from_client {
	uint8_t TestID[TESTID_LEN];
	peripheral_t Peripheral;
	uint8_t iter_num;
	uint8_t pattern_len;
	uint8_t str[BUFF_SIZE];
}packet_from_client;


typedef struct packet_to_client {
	uint8_t TestID[TESTID_LEN];
	uint8_t TestResult;

}packet_to_client;

#define RETURN_MESSAGE_LENGTH 5
/*
 *	EVB configuration:
 *	IP: 		192.168.7.2
 *	NETMASK: 	255.255.255.0 (or 192.168.7.0/24)
 *	GATEWAY: 	192.168.7.1 (not in use but required by IDE)
 *	Port:		50,007
 */


uint8_t callback_flag = 0;
ip_addr_t dest_ipaddr = {0};
u16_t dest_port = 0;
struct udp_pcb *upcb;

void Testing_Program(void) {
	uint8_t testing_result= TRUE;

	uint8_t incoming_buf[MAX_MSG_LEN]={0}; // incoming buffer. stores bit string from client

	packet_to_client server_to_client_packet={0};
	packet_from_client client_to_server_packet={0};

	uint8_t return_from_test_value[BUFF_SIZE]={0}; //returned string from test units



	printf("Start of program\r\n");
	udpServer_init();					//UDP server initialization
	while (1) {

		ethernetif_input(&gnetif);		//Handles the actual reception of bytes from the network interface
		sys_check_timeouts();			//Handle which checks timeout expiration
		if(callback_flag == 1){
			callback_flag=0;
			Parsing(&client_to_server_packet, incoming_buf);
			switch ((uint32_t)client_to_server_packet->Peripheral){
				case (TIMER): //start check from here
					for (int iter=0; iter<client_to_server_packet->iter_num;i++){
						timer_testing( returned_buff);
						testing_result= (strcmp(return_from_test_value,client_to_server_packet->str)!=0)? FALSE : TRUE ;
						if (testing_result==FALSE){

							printf("TIMER test failed!\n");
							break;


						}

					}
					break;

				case (UART):
					for (int iter=0; iter<client_to_server_packet->iter_num;i++){
						uart_testing(client_to_server_packet->str, return_from_test_value);
						testing_result= (strcmp(return_from_test_value,client_to_server_packet->str)!=0)? FALSE : TRUE ;
						if (testing_result==FALSE){
							printf("UART test failed!\n");
							break;


						}

					}
					break;
				case (SPI):
					for (int iter=0; iter<client_to_server_packet->iter_num;i++){
						spi_testing(client_to_server_packet->str, return_from_test_value);
						testing_result= (strcmp(return_from_test_value,client_to_server_packet->str)!=0)? FALSE : TRUE ;
						if (testing_result==FALSE){
							printf("SPI test failed!\n");
							break;


						}

					}
					break;
				case (I2C):
					for (int iter=0; iter<client_to_server_packet->iter_num;i++){
						i2c_testing(client_to_server_packet->str, return_from_test_value);
						testing_result= (strcmp(return_from_test_value,client_to_server_packet->str)!=0)? FALSE : TRUE ;
						if (testing_result==FALSE){
							printf("I2C test failed!\n");
							break;


						}

					}
					break;
				case (ADC_p):
					for (int iter=0; iter<client_to_server_packet->iter_num;i++){
						adc_testing(client_to_server_packet->str, return_from_test_value);
						testing_result= (strcmp(return_from_test_value,client_to_server_packet->str)!=0)? FALSE : TRUE ;
						if (testing_result==FALSE){
							printf("ADC test failed!\n");
							break;


						}

					}
					break;
				default:
					break;



			}
			memcpy((uint8_t*)server_to_client_packet->TestID,(uint8_t*)client_to_server_packet->TestID,4);
			memcpy((uint8_t*)&(server_to_client_packet->TestResult),(uint8_t*)&testing_result ,1);

		    err_t error= send_packet( pcb, (const void*) result_to_client, (u16_t) 5, (const ip_addr_t*) &dest_ipaddr, (u16_t) dest_port);

		    memset((uint8_t*)incoming_buff,0,sizeof(incoming_buff)); // incoming buffer. stores bit string from client
		    memset((packet_to_client*)&server_to_client_packet,0,sizeof(&server_to_client_packet));
		    memset((packet_from_client*)&client_to_server_packet,0,sizeof(&client_to_server_packet));
		    memset((uint8_t*)return_from_test_value,0,sizeof(return_from_test_value));





		}

	}
}
