//飞行员程序，负责定时读取传感器 和 计算 PID 和 控制电机

#include "TPilot.h"
#include "FlyBasic.h"

#include "Algorithm/StatusCalc.h"
#include "Algorithm/StatusCtrl.h"
#include "Algorithm/Plan.h"

#include "Hardware/LED.h"
#include "Hardware/XRotor.h"
#include "Hardware/Ultrasonic.h"

struct pt pTPilot;
PT_THREAD(TPilot(struct pt *pt));

void Init_TPilot(void) {
	PT_INIT(&pTPilot);
	
	SC_Init_All();	
	SCx_Init();
}

void Do_TPilot(void) {
	TPilot(&pTPilot);
}

static pt_timer init_until;
static uint8_t tp_initing = 0;

PT_THREAD(TPilot(struct pt *pt)) {
	PT_BEGIN(pt);
	
	 //十秒之内是用于陀螺仪的数据稳定的，可以用来做预采样
	init_until = millis() + 10000;
	while (millis() <= init_until) {
		PT_TIMER_INTERVAL(pt, 10);
		SC_PreSample();
		
		if (tp_initing & 0xF0) { LED_ON(2); } else { LED_OFF(2); }
		tp_initing++;
		
		PT_YIELD(pt);
	}
	SC_PreSample_End();
	
	LED_OFF(1);
	LED_OFF(2);
	LED_OFF(3);
	LED_OFF(4);
	
	Ultrasonic.callback = &SCx_ProcessAlt;
	
	//等到稳定下来以后，进入以下程序
	while(1) {
		PT_TIMER_INTERVAL(pt, 5);  //控制程序频率为 200 Hz
		
		SC_Sample(); 	//数据采样
		SC_Generate();	//姿态计算
		
		//读取结束后就交给 PID 处理了
		
		//Flight_Working
		// 0 -- 不动
		// 1 -- 准备预热
		// 2 -- 预热
		// 3 -- 普通飞行
		// 4 -- 降落
		
		switch (Flight_Working) {
			case FWS_IDLE: // 0 -- 不动
				Motor_SetAllSpeed(0,0,0,0);
				break;
			case FWS_PREPARE:	// 1 -- 准备预热
				Motor_SetAllSpeed(20,20,20,20);
				init_until = millis() + 6000;
				Flight_Working = FWS_WARMING;
				break;
			case FWS_WARMING: // 2 -- 预热
				if (millis() >= init_until) {
					Flight_Working = FWS_FLYING;
					Plan_Start();
				}
				break;
			case FWS_FLYING: // 3 -- 普通飞行
				SCx_ProcessAngle();
				SCx_ProcessOutput();
				break;
			case FWS_LANDING: // 4 -- 降落
				status_ctrl.expectedStatus.Altitude = 10;
				SCx_ProcessAngle();
				SCx_ProcessOutput();
				if (status.Altitude < 40 ){
					Flight_Working = FWS_IDLE;
				}
		}
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
