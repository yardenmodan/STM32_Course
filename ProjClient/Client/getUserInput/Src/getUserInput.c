void getUserInput(packet_field iter_field, uint8_t* chosen_field) {
	switch (iter_field) {
		case TEST_ID:
			HAL_UART_Transmit(UART_DEBUG, "Please enter Test ID:\r\n", strlen("Please enter Test ID:\r\n"), HAL_MAX_DELAY);
			break;

		case PERIPHERAL:
			HAL_UART_Transmit(UART_DEBUG, "Please enter Peripheral name (Timer, ADC, SPI, I2C, UART):\r\n", strlen("Please enter Peripheral name (Timer, ADC, SPI, I2C, UART):\r\n"), HAL_MAX_DELAY);
			break;

		case ITER_NUM:
			HAL_UART_Transmit(UART_DEBUG, "Please enter number of iterations to run (256 iterations tops):\r\n", strlen("Please enter number of iterations to run (256 iterations tops):\r\n"), HAL_MAX_DELAY);
			break;

		case PATTERN_LEN:
			HAL_UART_Transmit(UART_DEBUG, "Please enter string length (256 characters tops):\r\n", strlen("Please enter string length (256 characters tops):\r\n"), HAL_MAX_DELAY);
			break;

		case STR:
			HAL_UART_Transmit(UART_DEBUG, "Please enter a message (256 characters tops):\r\n", strlen("Please enter a message (256 characters tops):\r\n"), HAL_MAX_DELAY);
			break;

		default:
			break;
	}
	while (1){

		int iter_str=0;
		uint8_t* chosen_index=chosen_field+iter_str;
		HAL_UART_Receive(UART_DEBUG, chosen_index, 1, HAL_MAX_DELAY);

		if (*chosen_index == '\n') {

			break;
		} else {
			iter_str++;
		}
	}
}
