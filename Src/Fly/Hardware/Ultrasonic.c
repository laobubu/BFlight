#include "Ultrasonic.h"
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim4;

Ultrasonic_Typedef Ultrasonic;
float Ultrasonic_SpeedFactor = 58.0f; // us / 58 => cm

void Ultrasonic_Init(void) {
	HAL_TIM_Base_Stop(&htim4);
	GPIOE->BRR  |= 1 << 6;
	
	Ultrasonic.status = USS_IDLE;
	Ultrasonic.altitude = 0.0f;
}

void Ultrasonic_Trig(void) {
	unsigned short cntdown = 200;
	if (Ultrasonic.status == USS_IDLE) {
		htim4.Instance->CNT = 0;
		Ultrasonic.status = USS_PREPARE;
		
		GPIOE->BSRR |= 1 << 6;
		while(--cntdown);
		GPIOE->BRR  |= 1 << 6;
	}
}

void Ultrasonic_Echo(char high){
	if (high) { //超声波开始
		HAL_TIM_Base_Start(&htim4);
		Ultrasonic.status = USS_TIMING;
	} else { //超声波结束
		HAL_TIM_Base_Stop(&htim4);
		Ultrasonic.altitude = htim4.Instance->CNT / Ultrasonic_SpeedFactor;
		Ultrasonic.status = USS_IDLE;
	}
}

void Ultrasonic_TimeoutCallback(void) {
	HAL_TIM_Base_Stop(&htim4);
	Ultrasonic.status = USS_IDLE;
}
