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
void SystemClock_Config(uint8_t Clock_Freq);


UART_HandleTypeDef huart2;

uint8_t Flash_Latency = 0;

int main(void){


    char msg[100];
	HAL_Init();

	SystemClock_Config(SYS_CLOCK_120MHZ);

	UART2_Init();

	memset(msg,0,sizeof(msg));
	sprintf(msg,"SYSCLK : %ldMHz \r\n", HAL_RCC_GetSysClockFreq()/1000000);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg,0,sizeof(msg));
	sprintf(msg,"HCLK : %ldMHz \r\n", HAL_RCC_GetHCLKFreq()/1000000);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg,0,sizeof(msg));
	sprintf(msg,"PCLK1 : %ldMHz \r\n", HAL_RCC_GetPCLK1Freq()/1000000);
	HAL_UART_Transmit(&huart2,(uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg,0,sizeof(msg));
	sprintf(msg,"PCLK2 : %ldMHz \r\n", HAL_RCC_GetPCLK2Freq()/1000000);
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


	while(1);

}


void SystemClock_Config(uint8_t Clock_Freq){

	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.HSICalibrationValue = 16; //default value
    osc_init.PLL.PLLState = RCC_PLL_ON;
    osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI; // 16MHz

	switch(Clock_Freq){
		case SYS_CLOCK_50MHZ:{
			osc_init.PLL.PLLM = 16; //input of PLL engine should be 1MHz < x < 2MHz so 16/16 = 1Mhz
			osc_init.PLL.PLLN = 100; // now we have 100Mhz
			osc_init.PLL.PLLP = RCC_PLLP_DIV2; // here the output is 50Mhz
			osc_init.PLL.PLLQ = 2; //default

			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
						RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		    clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; //
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;  //  50/1 = 50MHz
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;   //  50/2 = 25MHz
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;   //  20/2 = 25MHz

			Flash_Latency = FLASH_ACR_LATENCY_1WS; // the values get from RM FLASH MEMORY section

			break;
		}
		case SYS_CLOCK_84MHZ:{
			osc_init.PLL.PLLM = 16; //input of PLL engine should be 1MHz < x < 2MHz so 16/16 = 1Mhz
			osc_init.PLL.PLLN = 168; // now we have 168Mhz
			osc_init.PLL.PLLP = RCC_PLLP_DIV2; // here the output is 84Mhz
			osc_init.PLL.PLLQ = 2; //default

			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
							RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		    clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; //
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;  //  84/1 = 84MHz
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;   //  84/2 = 42MHz
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;   //  84/2 = 42MHz

			Flash_Latency = FLASH_ACR_LATENCY_2WS;

			break;
		}
		case SYS_CLOCK_120MHZ:{
			osc_init.PLL.PLLM = 16; //input of PLL engine should be 1MHz < x < 2MHz so 16/16 = 1Mhz
			osc_init.PLL.PLLN = 240; // now we have 240Mhz
			osc_init.PLL.PLLP = RCC_PLLP_DIV2; // here the output is 120Mhz
			osc_init.PLL.PLLQ = 2; //default

			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
							RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; //
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;  //  120/1 = 120MHz
			clk_init.APB1CLKDivider = RCC_HCLK_DIV4;   //  120/4 = 30MHz  the max is 42Mhz
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;   //  120/2 = 60MHz  the max is 84Mhz

			Flash_Latency = FLASH_ACR_LATENCY_3WS;

			break;
		}
		default:
			return;
	}

	if( HAL_RCC_OscConfig(&osc_init) != HAL_OK ){

		ErrorHandler();
	}

	if( HAL_RCC_ClockConfig(&clk_init , Flash_Latency) != HAL_OK){

		ErrorHandler();
	}

	/* ****************SYSTICK CONFIGURATION********************/

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK); // here the prescaler can 1 and 8 here as default we choose 1

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

