#include "Ultrasonic.h"
#include "FlyBasic.h"

#define htim_sonar		htim4
#define GPIO_TRIG		GPIOF
#define GPIO_TRIG_PIN	5

extern TIM_HandleTypeDef htim_sonar;

Ultrasonic_Typedef Ultrasonic;
const float Ultrasonic_SpeedFactor = 58.0 / 4.0; // us / 58 => cm

#define USE_US_FILTER1

#ifdef USE_US_FILTER1
static const float filter_a = 1;
static const float filter_acom = 1.0f - filter_a;
void US_UPDATE(float val) {
	if (val > (Ultrasonic.altitude + 15))
		val = (Ultrasonic.altitude + 15);
	else if (val < (Ultrasonic.altitude - 15))
		val = (Ultrasonic.altitude - 15);
	
	//Ultrasonic.altitude = filter_a * (val) + filter_acom * Ultrasonic.altitude;
	Ultrasonic.altitude = 	val;
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
	HAL_TIM_Base_Stop(&htim_sonar);
	GPIO_TRIG->BSRR  |= (1 << GPIO_TRIG_PIN)<<16;
	
	Ultrasonic.status = USS_IDLE;
	Ultrasonic.altitude = 0.0f;
	Ultrasonic.callback = 0;
}

void Ultrasonic_Trig(void) {
	unsigned short cntdown = 200;
	if (Ultrasonic.status == USS_IDLE) {
		htim_sonar.Instance->CNT = 0;
		Ultrasonic.status = USS_PREPARE;
		
		GPIO_TRIG->BSRR |= 1 << GPIO_TRIG_PIN;
		while(--cntdown);
		GPIO_TRIG->BSRR  |= (1 << GPIO_TRIG_PIN)<<16;
	}
}

void Ultrasonic_Echo(char high){
	if (Ultrasonic.status == USS_IDLE) return;
	
	if (high) { //超声波开始
		HAL_TIM_Base_Start(&htim_sonar);
		Ultrasonic.status = USS_TIMING;
	} else { //超声波结束
		HAL_TIM_Base_Stop(&htim_sonar);
		US_UPDATE(htim_sonar.Instance->CNT / Ultrasonic_SpeedFactor);
		if (Ultrasonic.callback)
			(*Ultrasonic.callback)();
		Ultrasonic.status = USS_IDLE;
	}
}

void Ultrasonic_TimeoutCallback(void) {
	HAL_TIM_Base_Stop(&htim_sonar);
	Ultrasonic.status = USS_IDLE;
}
