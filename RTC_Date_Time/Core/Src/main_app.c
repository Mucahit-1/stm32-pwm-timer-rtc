
#include <string.h>
#include<stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "stm32f4xx_hal.h"
#include "main_app.h"


void GPIO_Init(void);
void Error_handler(void);
void UART2_Init(void);
void SystemClock_Config(uint8_t clock_freq);
void RTC_Init(void);
void RTC_CalendarConfig(void);
UART_HandleTypeDef huart2;
RTC_HandleTypeDef hrtc;
uint8_t Flash_Latency = 0;

void printmsg(char *format,...)
 {

	char str[80];

	/*Extract the the argument list using VA apis */
	va_list args;
	va_start(args, format);
	vsprintf(str, format,args);
	HAL_UART_Transmit(&huart2,(uint8_t *)str, strlen(str),HAL_MAX_DELAY);
	va_end(args);

 }

int main(void)
{



	HAL_Init();

	GPIO_Init();

	SystemClock_Config(SYS_CLOCK_50MHZ);

	UART2_Init();

	RTC_Init();

	printmsg("This is RTC calendar Test program\r\n");

	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
		printmsg("Woke up from STANDBY\r\n");
		HAL_GPIO_EXTI_Callback(0);
	}


	//RTC_CalendarConfig();

	//Enable the wakeup pin 1 in pwr_csr register
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

	printmsg("Went to STANDBY mode\r\n");
	HAL_PWR_EnterSTANDBYMode();

	while(1);


	return 0;
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

		Error_handler();
	}

	if( HAL_RCC_ClockConfig(&clk_init , Flash_Latency) != HAL_OK){

		Error_handler();
	}


}

void RTC_Init(void)
{
	//the clock is LSI which is 32khz
   hrtc.Instance = RTC;
   hrtc.Init.HourFormat =RTC_HOURFORMAT_12;
   hrtc.Init.AsynchPrediv = 0x7F;
   hrtc.Init.SynchPrediv = 0xF9;
   // these prescalers values to get 1hz
   hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
   hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW;
   hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

   if( HAL_RTC_Init(&hrtc) != HAL_OK)
   {
	   Error_handler();
   }
}



void  RTC_CalendarConfig(void)
{
	RTC_TimeTypeDef RTC_TimeInit;
	RTC_DateTypeDef RTC_DateInit;
	// configure the calendar for Time : 10:19:25 AM Date : 18 September 2023 monday


	RTC_TimeInit.Hours = 11;
	RTC_TimeInit.Minutes = 14;
	RTC_TimeInit.Seconds = 25;
	RTC_TimeInit.TimeFormat = RTC_HOURFORMAT12_AM;
	HAL_RTC_SetTime(&hrtc, &RTC_TimeInit,RTC_FORMAT_BIN);


	RTC_DateInit.Date = 18;
	RTC_DateInit.Month = RTC_MONTH_SEPTEMBER;
	RTC_DateInit.Year = 23;
	RTC_DateInit.WeekDay = RTC_WEEKDAY_MONDAY;

	HAL_RTC_SetDate(&hrtc,&RTC_DateInit,RTC_FORMAT_BIN);

}

void GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();


	GPIO_InitTypeDef ledgpio , buttongpio;

	ledgpio.Pin = GPIO_PIN_5;
	ledgpio.Mode = GPIO_MODE_OUTPUT_PP;
	ledgpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA,&ledgpio);

	buttongpio.Pin = GPIO_PIN_10;
	buttongpio.Mode = GPIO_MODE_IT_FALLING;
	buttongpio.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA,&buttongpio);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn,15,0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);


}



void UART2_Init(void)
{


	huart2.Instance = USART2;
	huart2.Init.BaudRate =115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX;


	if ( HAL_UART_Init(&huart2) != HAL_OK )
	{
		//There is a problem
		Error_handler();
	}

}


char* getDayofweek(uint8_t number)
{
	char *weekday[] = { "Monday", "TuesDay", "Wednesday","Thursday","Friday","Saturday","Sunday"};

	return weekday[number-1];
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
 void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

	 RTC_TimeTypeDef RTC_TimeRead;
	 RTC_DateTypeDef RTC_DateRead;

	 HAL_RTC_GetTime(&hrtc,&RTC_TimeRead,RTC_FORMAT_BIN);

	 HAL_RTC_GetDate(&hrtc,&RTC_DateRead,RTC_FORMAT_BIN);

	 printmsg("Current Time is : %02d:%02d:%02d\r\n",RTC_TimeRead.Hours,\
			 RTC_TimeRead.Minutes,RTC_TimeRead.Seconds);

	 printmsg("Current Date is : %02d-%2d-20%2d  <%s> \r\n",RTC_DateRead.Month,RTC_DateRead.Date,\
			 RTC_DateRead.Year,getDayofweek(RTC_DateRead.WeekDay));

}



void Error_handler(void)
{
	while(1);
}
