/*
 * main.c
 *
 *  Created on: Jul 27, 2023
 *      Author: mucahit
 */

#include <string.h>
#include "stm32f4xx_hal.h"
#include "main.h"

void SystemClockConfig(void);
void ErrorHandler(void);
void TIMER6_Init(void);
void GPIO_Init(void);


TIM_HandleTypeDef htimer6;

int main(void){

	HAL_Init();
	SystemClockConfig();

    GPIO_Init();
	TIMER6_Init();

	// timer starting with IT

	HAL_TIM_Base_Start_IT(&htimer6);

	while(1);

	return 0;
}



void TIMER6_Init(void){

	htimer6.Instance = TIM6;
	htimer6.Init.Prescaler = 16000; // the clock is 16Mhz so 16000000/16000 = 1000Mhz clock of counter
	//1/1000 = 0.001 this is the period of counter
	htimer6.Init.Period = 500-1;  // in the notes i explained why we do minus 1 // the max allowed value 65535

	if( HAL_TIM_Base_Init(&htimer6) != HAL_OK){

		ErrorHandler();
	}
}

void GPIO_Init(void){

	GPIO_InitTypeDef ledgpio;
	__HAL_RCC_GPIOD_CLK_ENABLE();
	ledgpio.Pin = GPIO_PIN_6;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOD, &ledgpio);

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){


	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_6);

}



void ErrorHandler(void){

	while(1);
}


void SystemClockConfig(void){


}
