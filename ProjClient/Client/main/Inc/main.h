#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

#define TESTID_LEN 4

typedef struct __attribute__((__packed__)) {
    uint32_t TestID;
    uint8_t Peripheral;
    uint8_t iter_num;
    uint8_t pattern_len;
    uint8_t str[256];
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

void getUserInput(packet_field iter_field, uint8_t* chosen_field, int* iter_str);
void translateStringsToPacket(packet_from_client* msg_packet, char* Test_ID_str, char* Peripheral_num_str, char* iter_nums_str, char* message_len_str, char* str);
void sendTestUDP(char* serializedPacket);

#endif  // MAIN_H
