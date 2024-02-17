void translateStringsToPacket(packet_from_client* msg_packet, char* Test_ID_str, char* Peripheral_num_str, char* iter_nums_str, char* message_len_str, char* str) {
    char *endptr = 0;
    msg_packet->TestID = (uint32_t)strtol(Test_ID_str, &endptr, 10); // add error handling!

    switch (Peripheral_num_str) {
        case "ADC":
            msg_packet->Peripheral = 16;
            break;
        case "Timer":
            msg_packet->Peripheral = 1;
            break;
        case "I2C":
            msg_packet->Peripheral = 8;
            break;
        case "SPI":
            msg_packet->Peripheral = 4;
            break;
        case "UART":
            msg_packet->Peripheral = 2;
            break;
        default:
            break;
    }

    msg_packet->iter_num = (uint8_t)strtol(iter_nums_str, &endptr, 10); // add error handling!
    msg_packet->pattern_len = (uint8_t)strtol(message_len_str, &endptr, 10); // add error handling!
    strncpy(msg_packet->str, str, 257);
}
