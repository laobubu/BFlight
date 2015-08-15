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

static uint8_t HeartbeatMode;
extern char systemReady;
void MENU_Summon(void);

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
//		DP_SendPack.aux1 = HyperCCD.nav_position;
//		DP_SendPack.aux2 = HyperCCD.run_out_of_line; 
//		DP_SendPack.aux3 = Flight_Working * 10 + plan.status;
//		DP_SendPack.aux4 = status_ctrl.ghostExpect.Yaw;
//		
//		DP_SendPack.aux5 = status_ctrl.expectedStatus.Pitch;
//		DP_SendPack.aux6 = status_ctrl.expectedStatus.Roll;
//		DP_SendPack.aux7 = status_ctrl.expectedStatus.Yaw;
//		DP_SendPack.aux8 = status_ctrl.expectedStatus.Altitude;
		
		
		DP_SendPack.aux1 = HyperCCD.nav_position;
		DP_SendPack.aux2 = status_ctrl.PID_roll.d_previous; 
		DP_SendPack.aux3 = status_ctrl.PID_roll.error_previous;
		DP_SendPack.aux4 = status_ctrl.PID_roll.integral;
		
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
		if (HeartbeatMode) {
			if (Flight_Working && ((DP_LastUpdate + 2000) < millis())) {
				Flight_Working = 0;
				SCx_Init();
				Motor_SetAllSpeed(0,0,0,0);
			}
		} else {
			//1 key 模式
			GPIOE->ODR ^= 1;	//led1
			if (systemReady > 2) {
				systemReady--;
			} else if (systemReady == 2) {
				MENU_Summon();
				systemReady = 1;
			}
		}
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
