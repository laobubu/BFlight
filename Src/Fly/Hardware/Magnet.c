#include "FlyBasic.h"
#include "Magnet.h"

extern TIM_HandleTypeDef htim9;

void Magnet_Init(void)
{
	//GPIO Init
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);
	Magnet_Idle();
}
