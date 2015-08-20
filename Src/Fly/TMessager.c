//TMessager - the messager and watchdog!
// 1. 定时发送最新姿态数据 
// 2. 检查最近一次通讯时间并做出失联时的反应

#include "TMessager.h"
#include "FlyBasic.h"
#include "stdio.h"
#include "string.h"
#include "DataPacker.h"
#include "sys.h"

#include "Algorithm/StatusCtrl.h"
#include "Algorithm/plan.h"

#include "Hardware/CCD.h"
#include "Hardware/XRotor.h"
#include "Hardware/Ultrasonic.h"
#include "Hardware/MS5611_I2C.h"
#include "Hardware/PX4Flow.h"
#include "Hardware/Laser.h"
#include "Hardware/LED.h"
#include "Hardware/HyperCCD.h"
#include "Hardware/ADNS3080.h"


extern UART_HandleTypeDef huart1;

extern PID_Typedef  pidRollE;
extern float pidRollE_Expect;

static uint8_t HeartbeatMode;
uint32_t start_Work1_since = 0;
void DP_HandleParamUpdate(char name[4], float value);

struct pt ptMessagerThread;
PT_THREAD(TMessagerThread(struct pt *pt));

void Init_MessagerThread(uint8_t enableHeartbeat) {
	PT_INIT(&ptMessagerThread);
	DP_Init();
	HeartbeatMode = enableHeartbeat;
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
//		
//		DP_SendPack.aux1 = HyperCCD.nav_position;
//		DP_SendPack.aux2 = HyperCCD.run_out_of_line; 
//		DP_SendPack.aux3 = Flight_Working * 10 + plan.status;
//		DP_SendPack.aux4 = status_ctrl.ghostExpect.Yaw;
		
		DP_SendPack.aux1 = HyperCCD.nav_position;
		DP_SendPack.aux2 = HyperCCD.run_out_of_line; 
		DP_SendPack.aux3 = Flight_Working * 10 + plan.status;
		DP_SendPack.aux4 = status_ctrl.ghostExpect.Yaw;
		
		DP_SendPack.aux5 = status_ctrl.expectedStatus.Pitch;
		DP_SendPack.aux6 = status_ctrl.expectedStatus.Roll;
		DP_SendPack.aux7 = status_ctrl.expectedStatus.Yaw;
		DP_SendPack.aux8 = status_ctrl.expectedStatus.Altitude;
		
		
		/*
		DP_SendPack.__ccdheader = 0xFF;
		for(uint16_t xxx = 0; xxx != 128; xxx++)
		{
			DP_SendPack.__ccd[xxx] = (
			CCD.data[xxx]==0xFF)?0xFE:CCD.data[xxx];
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
		if (start_Work1_since == 0) {
			if (!ESP_Read(Pin_KEY_1)) {
				start_Work1_since = millis() + 5000;
				LED_ON(1);
				LED_ON(2);
				LED_ON(3);
				LED_ON(4);
			}
		} else if (start_Work1_since == 1) {
			
		} else if (start_Work1_since < millis()) {
			start_Work1_since = 1;
			DP_HandleParamUpdate("Work", 1.0f);
				LED_OFF(1);
				LED_OFF(2);
				LED_OFF(3);
				LED_OFF(4);
		}
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
