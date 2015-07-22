#include "sys.h"

#include "FlyBasic.h"
#include "AlgorithmBasic.h"
#include "Plan.h"
#include "Algorithm/PID.h"

#include "Hardware/HyperCCD.h"

Plan_t plan;

PID_Typedef pidRollE;
float pidRollE_Expect = 60;


void stopflying(void);

void Plan_Init(void) {
	plan.currentPoint = 0;
	plan.pointCount = 0;
	plan.time_since = millis();
	PID_Init(&pidRollE , 		PID_MODE_DERIVATIV_CALC, 	0.005f);
	
	plan.isWorking = 1;
}

#define CURRENT (plan.points[plan.currentPoint])
#define NEXT    (plan.points[plan.currentPoint + 1])
void Plan_Process(void) {
	//如果现在不执行计划，则退出该函数
		if (!plan.isWorking) return;
	//在下面写计划就好了；
//	if (Plan_GetTime() <1000){
//	     status_ctrl.expectedStatus.Pitch = -7;
//	}
//	else if ((Plan_GetTime() >1000)&&(Plan_GetTime() <2000)){
//	     status_ctrl.expectedStatus.Pitch = -1;
//	}
//	else if ((Plan_GetTime() >2000)&&(Plan_GetTime() <6000)){
//	     status_ctrl.expectedStatus.Pitch = -4;
//	}
//	if (Plan_GetTime() >6000){
//	    Flight_Working = FWS_LANDING;
//	    plan.isWorking = 0;
//	}                  //一个简单的计划系统
	
	

//	status_ctrl.expectedStatus.Pitch = 3;
	if (HyperCCD.status.run_out_of_line == 0 ){
   status_ctrl.expectedStatus.Roll = PID_Postion_Cal(
			&pidRollE,
			pidRollE_Expect,
			HyperCCD.nav_position,
			0,
			HyperCCD.time
		);
	}
	else {
	     stopflying();
	}
 //status_ctrl.expectedStatus.Altitude = 30; 
//  status_ctrl.expectedStatus.Yaw = 30;	
	
// 停止飞行
//	Flight_Working = FWS_LANDING;
//	plan.isWorking = 0;
}

uint32_t Plan_GetTime(void)
{
	return (millis() - plan.time_since);
}


void stopflying(void){
   Flight_Working = FWS_LANDING;
		 plan.isWorking = 0;
}
