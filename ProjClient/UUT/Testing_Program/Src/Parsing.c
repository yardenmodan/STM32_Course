#include "Parsing.h"
#include "string.h"
#include <stdio.h>
#include "Testing_Program.h"

void Parsing(packet_from_client * client_to_server_packet,uint8_t * incoming_buf  ){
	memcpy((const uint8_t*)client_to_server_packet->TestID,(const uint8_t*)incoming_buf,(size_t)4);
	memcpy((const uint8_t*)&(client_to_server_packet->Peripheral),(const uint8_t*)(incoming_buf+4),(size_t)1);// check if its write to pass Peripheral like this
	memcpy((const uint8_t*)&(client_to_server_packet->iter_num),(const uint8_t*)(incoming_buf+5),(size_t)1);
	memcpy((const uint8_t*)&(client_to_server_packet->pattern_len),(const uint8_t*)(incoming_buf+6),(size_t)1);
	memcpy((const uint8_t*)client_to_server_packet->str,(const uint8_t*)(incoming_buf+7),(size_t)*pattern_len); //check again

}
