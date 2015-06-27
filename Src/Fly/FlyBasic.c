///////////////////////////////////////////
/// FlyBasic
/// 提供一些有用的函数

#include "FlyBasic.h"

extern TIM_HandleTypeDef htim6;	//TIM6 is used to delay x us
void FlyBasic_Init(void)
{
	HAL_TIM_Base_Start(&htim6);
}
void delay_us(const uint16_t us)
{
	htim6.Instance->CNT = 0;
	while (htim6.Instance->CNT < us);
}
