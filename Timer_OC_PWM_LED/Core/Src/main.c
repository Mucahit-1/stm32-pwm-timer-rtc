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
void UART2_Init(void);

uint8_t Flash_Latency = 0;

TIM_HandleTypeDef htimer2;
UART_HandleTypeDef huart2;


int main(void){

	HAL_Init();

	SystemClock_Config(SYS_CLOCK_50MHZ);

    GPIO_Init();

    UART2_Init();

	TIMER2_Init();

	if( HAL_TIM_PWM_Start(&htimer2, TIM_CHANNEL_1) != HAL_OK){

		ErrorHandler();
	}

	if( HAL_TIM_PWM_Start(&htimer2, TIM_CHANNEL_2) != HAL_OK){

		ErrorHandler();
	}

	if( HAL_TIM_PWM_Start(&htimer2, TIM_CHANNEL_3) != HAL_OK){

		ErrorHandler();
	}

	if( HAL_TIM_PWM_Start(&htimer2, TIM_CHANNEL_4) != HAL_OK){

		ErrorHandler();
	}

	return 0;
}


void TIMER2_Init(void){

	TIM_OC_InitTypeDef tim2pwm_init;

	htimer2.Instance = TIM2;
	htimer2.Init.Prescaler = 4999; // the clock is 50Mhz so we reduce it to 10khz
	htimer2.Init.Period = 100000-1;  // this will give us 1sec

	if( HAL_TIM_PWM_Init(&htimer2) != HAL_OK){

		ErrorHandler();
	}

	memset(&tim2pwm_init , 0 , sizeof(tim2pwm_init));

	tim2pwm_init.OCMode = TIM_OCMODE_PWM1;
	tim2pwm_init.OCPolarity = TIM_OCPOLARITY_HIGH;

	tim2pwm_init.Pulse = ( htimer2.Init.Period * 25 ) / 100; // this give us 25% PWM duty cycle
	if( HAL_TIM_PWM_ConfigChannel(&htimer2, &tim2pwm_init, TIM_CHANNEL_1) != HAL_OK){

		ErrorHandler();
	}

	tim2pwm_init.Pulse = ( htimer2.Init.Period * 45 ) / 100; // this give us 45% PWM duty cycle
	if( HAL_TIM_PWM_ConfigChannel(&htimer2, &tim2pwm_init, TIM_CHANNEL_2) != HAL_OK){

		ErrorHandler();
	}

	tim2pwm_init.Pulse = ( htimer2.Init.Period * 75 ) / 100; // this give us 75% PWM duty cycle
	if( HAL_TIM_PWM_ConfigChannel(&htimer2, &tim2pwm_init, TIM_CHANNEL_3) != HAL_OK){

		ErrorHandler();
	}

	tim2pwm_init.Pulse = ( htimer2.Init.Period * 95 ) / 100; // this give us 95% PWM duty cycle
	if( HAL_TIM_PWM_ConfigChannel(&htimer2, &tim2pwm_init, TIM_CHANNEL_4) != HAL_OK){

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




void ErrorHandler(void){

	while(1);
}


void SystemClock_Config(uint8_t Clock_Freq) {

	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI; // | RCC_OSCILLATORTYPE_LSE;
	osc_init.HSIState = RCC_HSI_ON;
	//osc_init.LSEState = RCC_LSE_ON;
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


