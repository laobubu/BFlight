#include "FlyBasic.h"

#include "Algorithm/StatusCalc.h"

void Ultrasonic_Echo(char high);
void Ultrasonic_TimeoutCallback(void);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_4) 
		Ultrasonic_Echo(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_4));
	
	if (GPIO_Pin == GPIO_PIN_8) // DMP.INTA
		SC_Callback_DMP();
	
	if (GPIO_Pin == GPIO_PIN_9) // HMC58X3.DRDY
		SC_Callback_HMC58X3();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM4)
		Ultrasonic_TimeoutCallback();
}

///WARNING! TO MAKE THINGS MORE EFFECTIVE,
/// THINGS ABOUT USART RX ARE WRITTEN IN `stm32f1xx_it.c`
