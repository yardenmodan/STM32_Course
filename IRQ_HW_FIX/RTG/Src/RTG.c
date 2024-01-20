#include "RTG.h"
#include "main.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define USER_Btn_Pin GPIO_PIN_13
#define USER_Btn_GPIO_Port GPIOC
#define USER_Btn_EXTI_IRQn EXTI15_10_IRQn
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
int button_flag=0;

void rtg_main(void){
	while (1){
		if (button_flag){
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
			button_flag=0;
		}

	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	button_flag=1;
}

void hw_main(){
	rtg_main();
}
