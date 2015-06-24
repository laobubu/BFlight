#include "TMessager.h"
#include "stdio.h"
#include "string.h"
#include "stm32f1xx_hal.h"
#include "DataPacker.h"

#include "Algorithm/StatusCalc.h"

extern UART_HandleTypeDef huart1;

struct pt ptMessagerThread;
PT_THREAD(TMessagerThread(struct pt *pt));

void Init_MessagerThread(void) {
	PT_INIT(&ptMessagerThread);
	DP_Init();
}

void Do_MessagerThread(void) {
	TMessagerThread(&ptMessagerThread);
}

PT_THREAD(TMessagerThread(struct pt *pt)) {
	PT_BEGIN(pt);
	
	while(1) {
		PT_TIMER_INTERVAL(pt, 100);	//Share data via USART @ 10 Hz
		
		DP_SendPack.Pitch = status.Pitch;
		DP_SendPack.Roll = status.Roll;
		DP_SendPack.Yaw = status.Yaw;
		DP_SendPack.Alt = status.Altitude;
		
		DP_Send();
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
