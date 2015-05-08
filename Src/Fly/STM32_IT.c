#include "stm32f1xx_hal.h"

extern char HMC58X3_DRDY;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_8) // MPU6050 - HMC5883 DRDY
		HMC58X3_DRDY = 1;
}
