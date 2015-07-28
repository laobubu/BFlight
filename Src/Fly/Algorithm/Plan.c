#include "sys.h"

#include "FlyBasic.h"
#include "AlgorithmBasic.h"
#include "Param.h"
#include "Plan.h"
#include "Algorithm/PID.h"

#include "Hardware/HyperCCD.h"

Plan_t plan;

PID_Typedef pidRollE;
PID_Typedef pidYawE;
float pidRollE_Expect = 73;
float pidYawE_Expect = 73;
int  left_flag = 0;
int  right_flag = 0 ;


void stopflying(void);

void Plan_Init(void) {
	PID_Init(&pidRollE , 		PID_MODE_DERIVATIV_CALC, 	0.005f);
  PID_Init(&pidYawE , 		PID_MODE_DERIVATIV_CALC, 	0.005f);
}

void Plan_Start(void) {
	plan.time_since = millis();
	plan.isWorking = 1;
	plan.status = (PLAN1_STATUS_TYPE)0;
}

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
	
	
	
	
	switch (plan.status) {
		case P1S_LIFT:
			//status_ctrl.expectedStatus.Altitude = 40;
			if (status.Altitude > 20) {
				plan.status = P1S_FOLLOW_LINE;
			}
			break;
			
		case P1S_FOLLOW_LINE:
//			if (HyperCCD.run_out_of_line == 1 ){
//				plan.status = P1S_RUN_OUT_OF_LINE;
//			}
			if (HyperCCD.turn_left== 1 ){
				plan.status = P1S_TURN_LEFT;
				left_flag  = 1;
			}
		  if (HyperCCD.turn_right== 1 ){
				plan.status = P1S_TURN_RIGHT;
				right_flag  = 1 ; 
			}
				if (HyperCCD_HasNewData()) {
					status_ctrl.expectedStatus.Roll = Param.RFix + PID_Postion_Cal(
						&pidRollE,
						pidRollE_Expect,
						HyperCCD.nav_position,
						0,
						HyperCCD.time
					);
//					status_ctrl.expectedStatus.Yaw =  Param.YFix + PID_Postion_Cal(&pidYawE ,	pidYawE_Expect,HyperCCD.nav_position,0,HyperCCD.time);
				}
			
		 break;
		case P1S_TURN_LEFT:		if(left_flag == 1){
			  status_ctrl.expectedStatus.Yaw -= 90;
			   left_flag = 0;
			}

 if ((!HyperCCD.run_out_of_line)&&(fabsf(status.Yaw - status_ctrl.expectedStatus.Yaw )< 5)&&(!HyperCCD.turn_left)&&(!HyperCCD.turn_right)){
            plan.status = P1S_FOLLOW_LINE;
     } 
		break;
		case P1S_TURN_RIGHT:
			if(right_flag == 1){
			status_ctrl.expectedStatus.Yaw +=  90;
			right_flag  = 0 ; 
			}
	 if ((!HyperCCD.run_out_of_line)&&(fabsf(status.Yaw - status_ctrl.expectedStatus.Yaw )< 5)&&(!HyperCCD.turn_left)&&(!HyperCCD.turn_right) ){
			plan.status = P1S_FOLLOW_LINE;
	     } 
		break;
		//case P1S_RUN_OUT_OF_LINE:
      // stopflying();
		//break;          //停止的问题再议。
				
	}
}
	
	



uint32_t Plan_GetTime(void)
{
	return (millis() - plan.time_since);
}


void stopflying(void){

	Flight_Working = FWS_IDLE;
	plan.isWorking = 0;
	plan.status = P1S_LIFT ;
	
}
