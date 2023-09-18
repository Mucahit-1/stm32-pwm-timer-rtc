/*
 * it.c
 *
 *  Created on: Jul 27, 2023
 *      Author: mucahit
 */

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htimer6;


void SysTick_Handler(void){

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM6_DAC_IRQHandler(){

	HAL_GPIO_WritePin(GPIOA ,GPIO_PIN_12, GPIO_PIN_SET);
	HAL_TIM_IRQHandler(&htimer6);
	HAL_GPIO_WritePin(GPIOA ,GPIO_PIN_12, GPIO_PIN_RESET);

}

