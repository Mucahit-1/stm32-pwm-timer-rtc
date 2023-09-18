/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
uint8_t Flash_Latency = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(uint8_t Clock_Freq);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	uint32_t *pBackupSRAMBase = 0;
	char write_buf[] = "Hello";
	HAL_Init();

	SystemClock_Config(SYS_CLOCK_50MHZ);

	MX_GPIO_Init();

	MX_USART2_UART_Init();

	//1. enable the clock in RCC register for backup SRAM
	__HAL_RCC_BKPSRAM_CLK_ENABLE();

	//2.enable write access to the backup domain
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();

	pBackupSRAMBase = (uint32_t *) BKPSRAM_BASE; // to write to it  it is 4KB size

	for(uint32_t i = 0 ; i < strlen(write_buf)+1 ; i++){

		*(pBackupSRAMBase + i) = write_buf[i];
	}

	while (1);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
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


/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    ErrorHandler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef buttongpio;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();


  buttongpio.Pin = GPIO_PIN_13;
  buttongpio.Mode = GPIO_MODE_INPUT;
  buttongpio.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &buttongpio);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void ErrorHandler(void)
{

  while (1);

}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

