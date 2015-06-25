//TMessager - the messager and watchdog!
// 1. 定时发送最新姿态数据 
// 2. 检查最近一次通讯时间并做出失联时的反应

#include "TMessager.h"
#include "stdio.h"
#include "string.h"
#include "stm32f1xx_hal.h"
#include "DataPacker.h"

#include "Algorithm/StatusCalc.h"
#include "Hardware/XRotor.h"
#include "Hardware/PX4Flow.h"

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
		
		DP_SendPack.Motor[0] = Motor_Out[0];
		DP_SendPack.Motor[1] = Motor_Out[1];
		DP_SendPack.Motor[2] = Motor_Out[2];
		DP_SendPack.Motor[3] = Motor_Out[3];
		
		DP_SendPack.optX = PX4Flow.x;
		DP_SendPack.optY = PX4Flow.x;
		
		DP_Send();
		
		//失联检测
		if (Flight_Working && ((DP_LastUpdate + 2000) < millis())) {
			Flight_Working = 0;
			PID_Init_All();
			Motor_SetAllSpeed(0,0,0,0);
		}
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
