#include "Ultrasonic.h"
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim4;

Ultrasonic_Typedef Ultrasonic;
const float Ultrasonic_SpeedFactor = 58.0 / 4.0; // us / 58 => cm

#define USE_US_FILTER1

#ifdef USE_US_FILTER1
static const float filter_a = 0.1;
static const float filter_acom = 1.0f - filter_a;
void US_UPDATE(float val) {
	if (val > (Ultrasonic.altitude + 20))
		val = (Ultrasonic.altitude + 20);
	else if (val < (Ultrasonic.altitude - 20))
		val = (Ultrasonic.altitude - 20);
	
	Ultrasonic.altitude = filter_a * (val) + filter_acom * Ultrasonic.altitude;
}

#elif defined(USE_US_FILTER2)
static float filter_valhold[4];
static uint8_t filter_valindex = 0;
void US_UPDATE(float val) {
	filter_valhold[filter_valindex++] = val; 
	if (filter_valindex>=4) filter_valindex=0;
	Ultrasonic.altitude = 0;	
	for (uint8_t xxaa = 0; xxaa<4;xxaa++)
		if (Ultrasonic.altitude < filter_valhold[xxaa])
			Ultrasonic.altitude = filter_valhold[xxaa];
}
#else
#define US_UPDATE(val) Ultrasonic.altitude = val
#endif

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
	if (Ultrasonic.status == USS_IDLE) return;
	
	if (high) { //超声波开始
		HAL_TIM_Base_Start(&htim4);
		Ultrasonic.status = USS_TIMING;
	} else { //超声波结束
		HAL_TIM_Base_Stop(&htim4);
		US_UPDATE(htim4.Instance->CNT / Ultrasonic_SpeedFactor);
		Ultrasonic.status = USS_IDLE;
	}
}

void Ultrasonic_TimeoutCallback(void) {
	HAL_TIM_Base_Stop(&htim4);
	Ultrasonic.status = USS_IDLE;
}
