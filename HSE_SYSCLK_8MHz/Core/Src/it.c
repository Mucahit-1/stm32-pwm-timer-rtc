/*
 * it.c
 *
 *  Created on: Jul 27, 2023
 *      Author: mucahit
 */

#include "main.h"


void SysTick_Handler(void){

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


