//TMessager - the messager and watchdog!
// 1. 定时发送最新姿态数据 
// 2. 检查最近一次通讯时间并做出失联时的反应

#include "TMessager.h"
#include "FlyBasic.h"
#include "stdio.h"
#include "string.h"
#include "DataPacker.h"

#include "Algorithm/StatusCtrl.h"
#include "Algorithm/plan.h"

#include "Hardware/CCD.h"
#include "Hardware/XRotor.h"
#include "Hardware/Ultrasonic.h"
#include "Hardware/MS5611_I2C.h"
#include "Hardware/PX4Flow.h"
#include "Hardware/Laser.h"
#include "Hardware/HyperCCD.h"
#include "Hardware/ADNS3080.h"


extern UART_HandleTypeDef huart1;

extern PID_Typedef  pidRollE;
extern float pidRollE_Expect;

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
		
		DP_SendPack.Motor[0] = status_ctrl.Motor_Out[0];
		DP_SendPack.Motor[1] = status_ctrl.Motor_Out[1];
		DP_SendPack.Motor[2] = status_ctrl.Motor_Out[2];
		DP_SendPack.Motor[3] = status_ctrl.Motor_Out[3];
		
		DP_SendPack.aux1 = status_ctrl.expectedStatus.Roll;
		DP_SendPack.aux2 = status_ctrl.expectedStatus.Pitch; 
		DP_SendPack.aux3 = ADNS3080.sumX;
		DP_SendPack.aux4 = ADNS3080.sumY;
		
		/*
		DP_SendPack.__ccdheader = 0xFF;
		for(uint16_t xxx = 0; xxx != 128; xxx++)
		{
			DP_SendPack.__ccd[xxx] = (CCD.data[xxx]==0xFF)?0xFE:CCD.data[xxx];
		}
		HAL_UART_Transmit_DMA(&huart1, (uint8_t*)&DP_SendPack.__ccdheader, 129);
		*/
		
		DP_Send();
		
		//失联检测
		if (Flight_Working && ((DP_LastUpdate + 2000) < millis())) {
			Flight_Working = 0;
			SCx_Init();
			Motor_SetAllSpeed(0,0,0,0);
		}
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
