/*
 * main.c
 *
 *  Created on: Jul 27, 2023
 *      Author: mucahit
 */

#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "main.h"

void SystemClock_Config(uint8_t Clock_Freq);
void ErrorHandler(void);
void TIMER2_Init(void);
void GPIO_Init(void);
void LSE_Configuration(void);
void UART2_Init(void);


TIM_HandleTypeDef htimer2;
UART_HandleTypeDef huart2;

uint8_t Flash_Latency = 0;
uint32_t input_captures[2] = {0};
uint8_t count = 1;
uint8_t is_capture_done = FALSE;

int main(void){

	uint32_t capture_difference = 0;
	double timer2_count_resolution = 0;
	double timer2_count_freq = 0;
	double user_signal_time_period = 0;
	double user_signal_freq = 0;
	char usr_msg[100];

	HAL_Init();
	SystemClock_Config(SYS_CLOCK_50MHZ);

    GPIO_Init();
	TIMER2_Init();
	LSE_Configuration();

	// timer starting
	HAL_TIM_IC_Start_IT(&htimer2, TIM_CHANNEL_1);


	while(1){

		if(is_capture_done){

			if(input_captures[1] > input_captures[0]){

				capture_difference = input_captures[1] - input_captures[0];
			}
			else{

				capture_difference = (0xFFFFFFFF - input_captures[0]) + input_captures[1];

			}

			timer2_count_freq = ( HAL_RCC_GetPCLK1Freq() * 2 ) / (htimer2.Init.Prescaler+1); //prescalar value ; this value we get from clock tree
			timer2_count_resolution = 1 / timer2_count_freq;
			user_signal_time_period = capture_difference * timer2_count_resolution;
			user_signal_freq = 1 / user_signal_time_period;
			sprintf(usr_msg , "Frequency of the signal applied = %f\r\n" , user_signal_freq);
			HAL_UART_Transmit(&huart2,(uint8_t*)usr_msg , strlen(usr_msg), HAL_MAX_DELAY);

			is_capture_done = FALSE;
		}
	}

	return 0;
}


void TIMER2_Init(void){

	TIM_IC_InitTypeDef timer2_config;

	htimer2.Instance = TIM2;
	htimer2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htimer2.Init.Prescaler = 1; // the clock is 50Mhz so the clock of counter is 25Mhz
	htimer2.Init.Period = 0xFFFFFFFF;  // in TIM2 the ARR IS 32 bit so we put the max allowed value to count from 0 to it

	timer2_config.ICFilter = 0;
	timer2_config.ICPolarity = TIM_ICPOLARITY_RISING;
	timer2_config.ICPrescaler = TIM_ICPSC_DIV1;
	timer2_config.ICSelection = TIM_ICSELECTION_DIRECTTI;

	if( HAL_TIM_IC_ConfigChannel(&htimer2, &timer2_config, TIM_CHANNEL_1 ) != HAL_OK ){

		ErrorHandler();
	}

	if( HAL_TIM_IC_Init(&htimer2) != HAL_OK){

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


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

	if(! is_capture_done){

		if(count == 1){

			input_captures[0] = __HAL_TIM_GET_COMPARE(htim , TIM_CHANNEL_1);
			count++;
		}
		else if(count == 2){

			input_captures[1] = __HAL_TIM_GET_COMPARE(htim , TIM_CHANNEL_1);
			count = 1;
			is_capture_done = TRUE;

		}
	}
}


void ErrorHandler(void){

	while(1);
}


void SystemClock_Config(uint8_t Clock_Freq) {

	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.LSEState = RCC_LSE_ON;
	osc_init.HSICalibrationValue = 16; //default value
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI; // 16MHz

	switch (Clock_Freq) {
	case SYS_CLOCK_50MHZ: {
		osc_init.PLL.PLLM = 16; //input of PLL engine should be 1MHz < x < 2MHz so 16/16 = 1Mhz
		osc_init.PLL.PLLN = 100; // now we have 100Mhz
		osc_init.PLL.PLLP = RCC_PLLP_DIV2; // here the output is 50Mhz
		osc_init.PLL.PLLQ = 2; //default

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
		RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; //
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;  //  50/1 = 50MHz
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;   //  50/2 = 25MHz
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;   //  20/2 = 25MHz

		Flash_Latency = FLASH_ACR_LATENCY_1WS; // the values get from RM FLASH MEMORY section

		break;
	}
	case SYS_CLOCK_84MHZ: {
		osc_init.PLL.PLLM = 16; //input of PLL engine should be 1MHz < x < 2MHz so 16/16 = 1Mhz
		osc_init.PLL.PLLN = 168; // now we have 168Mhz
		osc_init.PLL.PLLP = RCC_PLLP_DIV2; // here the output is 84Mhz
		osc_init.PLL.PLLQ = 2; //default

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
		RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; //
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;  //  84/1 = 84MHz
		clk_init.APB1CLKDivider = RCC_HCLK_DIV2;   //  84/2 = 42MHz
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2;   //  84/2 = 42MHz

		Flash_Latency = FLASH_ACR_LATENCY_2WS;

		break;
	}
	case SYS_CLOCK_120MHZ: {
		osc_init.PLL.PLLM = 16; //input of PLL engine should be 1MHz < x < 2MHz so 16/16 = 1Mhz
		osc_init.PLL.PLLN = 240; // now we have 240Mhz
		osc_init.PLL.PLLP = RCC_PLLP_DIV2; // here the output is 120Mhz
		osc_init.PLL.PLLQ = 2; //default

		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
		RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
		clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; //
		clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;  //  120/1 = 120MHz
		clk_init.APB1CLKDivider = RCC_HCLK_DIV4; //  120/4 = 30MHz  the max is 42Mhz
		clk_init.APB2CLKDivider = RCC_HCLK_DIV2; //  120/2 = 60MHz  the max is 84Mhz

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


}

void LSE_Configuration(void){

#if 0
	RCC_OscInitTypeDef Osc_Init;

	Osc_Init.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	Osc_Init.LSEState = RCC_LSE_ON;

	if( HAL_RCC_OscConfig(&Osc_Init) != HAL_OK){

		ErrorHandler();
	}

#endif

	// configuring the output pin of the clock
	HAL_RCC_MCOConfig(RCC_MCO1 , RCC_MCO1SOURCE_LSE , RCC_MCODIV_1);

}

