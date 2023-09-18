/*
 * it.c
 *
 *  Created on: Jul 27, 2023
 *      Author: mucahit
 */

#include "stm32f4xx_hal.h"


void SysTick_Handler(void){

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
