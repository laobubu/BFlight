#include "XRotor.h"

extern TIM_HandleTypeDef XROTOR_htim;

/**
  *
  * @brief 
  * 
  * @params 
  * @note 
  */

void Motor_SetAllSpeed(int16_t pwm1, int16_t pwm2, int16_t pwm3, int16_t pwm4) {
	if (pwm1<0) pwm1=0; else if (pwm1>MOTOR_PWM_MAX) pwm1 = MOTOR_PWM_MAX;
	if (pwm2<0) pwm2=0; else if (pwm2>MOTOR_PWM_MAX) pwm2 = MOTOR_PWM_MAX;
	if (pwm3<0) pwm3=0; else if (pwm3>MOTOR_PWM_MAX) pwm3 = MOTOR_PWM_MAX;
	if (pwm4<0) pwm4=0; else if (pwm4>MOTOR_PWM_MAX) pwm4 = MOTOR_PWM_MAX;
	
	Motor_SetSpeed(1,pwm1);
	Motor_SetSpeed(2,pwm2);
	Motor_SetSpeed(3,pwm3);
	Motor_SetSpeed(4,pwm4);
}

/**
  * void Motor_Init(void)
  *
  * @brief 初始化 PWM 的输出
  *
  * @note 应使用 STM32Cube 配置好频率为 50 Hz（周期20ms），重装值为 1999
  * @note 应用于 FreeRTOS 中
  */
void Motor_Init(void) {
  HAL_TIM_PWM_Start(&XROTOR_htim, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&XROTOR_htim, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&XROTOR_htim, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&XROTOR_htim, TIM_CHANNEL_4);
}

/**
  * void Motor_Configure_Start(void)
  *
  * @brief 进入配置模式
  *        
  * @note 初始化之后立即使用！直接输出2ms脉冲的样子
  */
void Motor_Configure_Start(void) {
  Motor_SetAllSpeed(100,100,100,100);
}


/**
  * void Motor_Configure_Confrim(void)
  *
  * @brief 确认配置
  *        
  * @note 在听到不同提示音发出后三秒内调用
  */
void Motor_Configure_Confrim(void) {
  Motor_SetAllSpeed(0,0,0,0);
}


