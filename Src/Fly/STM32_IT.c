#include "stm32f1xx_hal.h"

extern char HMC58X3_DRDY;

void Ultrasonic_Echo(char high);
void Ultrasonic_TimeoutCallback(void);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_4) 
		Ultrasonic_Echo(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4));
	
	if (GPIO_Pin == GPIO_PIN_8) // MPU6050 - HMC5883 DRDY
		HMC58X3_DRDY = 1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM4)
		Ultrasonic_TimeoutCallback();
}
