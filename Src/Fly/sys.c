#include "sys.h"

//max: 1hour
volatile uint32_t _millis = 0, _micros = 0;


///////////// ST HAL Driver
#if defined(STM32F4) || defined(STM32F1)
void ES_Init(void)
{
	SysTick->LOAD = (0xFFFFFF & SysTick->LOAD) / (1000/ES_TickInterval);
}
#endif




//////////// Renesas RL78 Driver
#if defined(RL78)
#include "r_cg_timer.h"
void ES_Init(void)
{
	R_TAU0_Create();
	R_TAU0_Channel6_Start();
}
#endif






void ESC_SysTick(void){
	static uint16_t _millis_cnt = 0;
	_micros += ES_TickInterval;
	_millis_cnt++;
	if (_millis_cnt == (1000/ES_TickInterval)) {
		_millis++;
		_millis_cnt = 0;
	}
}




uint32_t millis(void){ return _millis;}
uint32_t micros(void){ return _micros;}
void delay_ms(uint16_t ms){
	uint32_t target = _millis + ms;
	while(_millis<target);
}
void delay_us(uint16_t us){
	uint32_t since = _micros;
	uint32_t target = since + us;
	while((_micros < since) ^ (_micros < target));
}
