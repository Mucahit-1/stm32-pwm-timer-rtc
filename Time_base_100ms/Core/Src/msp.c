/*
 * msp.c
 *
 *  Created on: Jul 27, 2023
 *      Author: mucahit
 */

#include "stm32f4xx_hal.h"

void HAL_MspInit(void)
{
	// here we do low level processor specific init

	//1. set up the priority grouping of arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); // this is by default we can leave it

	//2.enable the required system exception of arm cortex mx processor
    SCB->SHCSR |= ( 0x7 << 16 );
    //here we put 111 in bit positions 16 ,17 ,18 to enable usage fault , memory fault and bus fault system exception
    // SCB is system control block and it is have some processor registers
    //SHCSR is system handler control and status register
    // we see all this information in the user guide of arm cortex m4


	//3. configure the priority for the system exception
    HAL_NVIC_SetPriority(MemoryManagement_IRQn,0,0);
    HAL_NVIC_SetPriority(BusFault_IRQn,0,0);
    HAL_NVIC_SetPriority(UsageFault_IRQn,0,0);

}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimer6){

	__HAL_RCC_TIM6_CLK_ENABLE();

	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);

}

