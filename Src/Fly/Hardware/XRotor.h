#ifndef __FLY_MOTOR_H
#define __FLY_MOTOR_H

#include "FlyBasic.h"

#define MOTOR_PWM_MAX 1000
#define XROTOR_htim	htim1		//STM32 HAL
#define XROTOR_TIM	(TIM1)

void Motor_Init(void);
void Motor_Configure_Start(void);
void Motor_Configure_Confrim(void);

#define Motor_SetSpeed(channel, percent) \
          XROTOR_TIM -> CCR##channel = (4000+(percent<<2)) //between 1ms to 2ms

void Motor_SetAllSpeed(int16_t pwm1, int16_t pwm2, int16_t pwm3, int16_t pwm4);

#endif
