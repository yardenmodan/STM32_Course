#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usart.h"

extern UART_HandleTypeDef huart3;

// printf
int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart3, (uint8_t*) &ch, 1, 0xFFFF);
	return ch;
}

int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart3, (uint8_t*) ptr, len, 0xFFFF);
	return len;
}

// scanf
int __io_getchar(void) {
	uint8_t ch = 0;
	HAL_UART_Receive(&huart3, &ch, 1, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart3, &ch, 1, HAL_MAX_DELAY);
	return ch;
}

int _read(int file, char *ptr, int len) {
	int DataIdx = 0;
	char ch;
	int counter = 0;
	for (; DataIdx < len; DataIdx++) {
		ch = __io_getchar();
		*ptr = ch;
		ptr++;
		if (ch == '\r' || ch == '\n') {
			*(ptr) = 0;
			break;
		}
	}
	return DataIdx + 1;
}
