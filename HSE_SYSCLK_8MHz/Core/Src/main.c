/*
 * main.c
 *
 *  Created on: Jul 27, 2023
 *      Author: mucahit
 */
#include <stdio.h>
#include <string.h>
#include "main.h"

#define TRUE 1
#define FALSE 0

void UART2_Init(void);
void ErrorHandler(void);


UART_HandleTypeDef huart2;


int main(void){

	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
    char msg[100]; // lets say it is 100 bytes

	HAL_Init();
	UART2_Init();

	memset(&osc_init,0,sizeof(osc_init)); // to make sure the values are zero
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_ON;
	if( HAL_RCC_OscConfig(&osc_init) != HAL_OK ){

		ErrorHandler();
	}

	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
			RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_HSE; // 8MHz
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2; //   8/2 = 4MHz
	clk_init.APB1CLKDivider = RCC_HCLK_DIV2;  //   4/2 = 2MHz
	clk_init.APB2CLKDivider = RCC_HCLK_DIV2;  //   4/2 = 2MHz

	if ( HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_0WS) != HAL_OK){
		// the flash latency value we take it from RM

		ErrorHandler();
	}
	//after this line the system clock will be sourced by the HSE instead of HSI.
	__HAL_RCC_HSI_DISABLE(); // now we can turn off HSI because it is not a system clock and we can save some current

	/* ****************SYSTICK CONFIGURATION********************/

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	//here we calculate when the HCLK IS 4MHz

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK); // here the prescaler can 1 and 8 here as default we choose 1

	UART2_Init(); // re-init uart because we changed the clock source


	memset(msg,0,sizeof(msg));
	sprintf(msg,"SYSCLK %ld \r\n", HAL_RCC_GetSysClockFreq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg,0,sizeof(msg));
	sprintf(msg,"HCLK %ld \r\n", HAL_RCC_GetHCLKFreq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg,0,sizeof(msg));
	sprintf(msg,"SYSCLK %ld \r\n", HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart2,(uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg,0,sizeof(msg));
	sprintf(msg,"SYSCLK %ld \r\n", HAL_RCC_GetPCLK2Freq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);



	while(1);

}


void UART2_Init(void){

	huart2.Instance = USART2; // here to link it to base address
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	if ( HAL_UART_Init(&huart2) != HAL_OK){

		// this mean there is a problem
		ErrorHandler();

	}

}


void ErrorHandler(void){

	while(1);
}


