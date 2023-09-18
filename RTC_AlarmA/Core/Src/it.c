/*
 * it.c
 *
 *  Created on: Sep 17, 2023
 *      Author: riyad
 */


#include "main_app.h"

extern RTC_HandleTypeDef hrtc;
void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();

}


void EXTI15_10_IRQHandler(void)
{

	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
}

void RTC_Alarm_IRQHandler(void)
{

	HAL_RTC_AlarmIRQHandler(&hrtc);
}
