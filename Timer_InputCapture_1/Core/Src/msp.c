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


void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim){

	GPIO_InitTypeDef ChannelConfig;

	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();


	// configure gpio to work as TIM2 channel 1

	ChannelConfig.Alternate = GPIO_AF1_TIM2;
	ChannelConfig.Mode = GPIO_MODE_AF_PP;
	ChannelConfig.Pull = GPIO_NOPULL;
	ChannelConfig.Pin = GPIO_PIN_15;
	ChannelConfig.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOA, &ChannelConfig);

	// NVIC configuration

	HAL_NVIC_EnableIRQ(TIM2_IRQn);

    HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);

}



void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart;
    // here to do UART peripheral low level inits
	// 1. Enabling the clock for USART2 peripheral
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

	// 2. Do the pin muxing configurations
    gpio_uart.Pin = GPIO_PIN_2;
    gpio_uart.Mode = GPIO_MODE_AF_PP;
    gpio_uart.Pull = GPIO_PULLUP; // in most communication protocol we use pull up
    gpio_uart.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_uart.Alternate = GPIO_AF7_USART2; // UART2_TX
    HAL_GPIO_Init(GPIOA, &gpio_uart);

    gpio_uart.Alternate = GPIO_AF7_USART2; // UART2_RX
    HAL_GPIO_Init(GPIOA, &gpio_uart);

	// 3. Enable the IRQ and set up the priority ( NVIC settings )
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);

}
