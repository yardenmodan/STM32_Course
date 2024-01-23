#include "Testing_Program.h"
#include "Server.h"
#include <string.h>
#include "Parsing.h"
#include "main.h"








/*
 *	EVB configuration:
 *	IP: 		192.168.7.2
 *	NETMASK: 	255.255.255.0 (or 192.168.7.0/24)
 *	GATEWAY: 	192.168.7.1 (not in use but required by IDE)
 *	Port:		50,007
 */




void Testing_Program(void) {

	const uint8_t TIMER =1;
	const uint8_t UART=2;
	const uint8_t SPI=4;
	const uint8_t I2C = 8;
	const uint8_t ADC_P = 16;


	packet_to_client server_to_client_packet={0};
	packet_from_client client_to_server_packet={0};

	uint8_t return_from_test_value[MAX_MSG_SIZE]={0}; //returned string from test units



	printf("Start of program\r\n");
	udpServer_init();					//UDP server initialization
	while (1) {
		uint8_t testing_result= TRUE;


		ethernetif_input(&gnetif);		//Handles the actual reception of bytes from the network interface
		sys_check_timeouts();			//Handle which checks timeout expiration
		if(callback_flag == 1){			// got packet
			callback_flag=0;
			Parsing(&client_to_server_packet, incoming_buffer);
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
