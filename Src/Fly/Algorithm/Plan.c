#include "sys.h"
#include "string.h"

#include "FlyBasic.h"
#include "AlgorithmBasic.h"
#include "Param.h"
#include "Plan.h"
#include "StatusCalc.h"
#include "Algorithm/PID.h"

#include "Hardware/HyperCCD.h"
#include "Hardware/Magnet.h"


Plan_t plan;
static uint32_t realPlanStartTime = 0;

void stopflying(void);

void Plan_Init(void) {
	Magnet_Init();
	PID_Init(&plan.pid.follow_line , 		PID_MODE_DERIVATIV_CALC, 	0.005f);
}

void Plan_Start(void) {
	Plan_StartTime();
	plan.isWorking = 1;
	plan.status = (PLAN1_STATUS_TYPE)0;
	realPlanStartTime = millis();
	
	pid_Reset_Integral(&plan.pid.follow_line);
	
	memset(&plan.aux, 0, sizeof(plan.aux));
}

void Plan_Prestart(void)
{
	if (Param.Mode == 3) {
			Magnet_Get();
	}
}

static void plan_do_follow_line(void) {
	if (HyperCCD_HasNewData()) {
		status_ctrl.expectedStatus.Roll = Param.RFix + PID_Postion_Cal(
			&plan.pid.follow_line,
			plan.pid.follow_line_expect,
			HyperCCD.nav_position,
			0,
			(float)HyperCCD.time / 1000.0f
		);
	}
}

void Plan_Process(void) {
	//如果现在不执行计划，则退出该函数
	if (!plan.isWorking || Flight_Working != FWS_FLYING) return;
	//在下面写计划就好了；

	
	if (Param.Tmot != 0 && (millis() - realPlanStartTime) >= Param.Tmot) {
		stopflying();
		return;
	}


	if (Param.Mode == 1)	// 模式1 的计划 
	{
		switch (plan.status) {
			case P1S_LIFT:
				status_ctrl.expectedStatus.Pitch = Param.PiGo;
				if (status.Altitude > 20 && Plan_GetTime() > 0) {
					plan.status = P1S_FOLLOW_LINE;
				}
				break;
				
			case P1S_FOLLOW_LINE:
			status_ctrl.expectedStatus.Pitch = Param.PFix;
			//status_ctrl.expectedStatus.Roll  = Param.RFix;
				if (HyperCCD.run_out_of_line == 1 ){
					plan.status = P1S_RUN_OUT_OF_LINE;
				} else {
					plan_do_follow_line();
				}
				
			 break;

			case P1S_RUN_OUT_OF_LINE:
				 stopflying();
			break;          //停止的问题再调。
					
		}  ////以上是巡线程序；
		
	} 
	else if (Param.Mode == 2)	// 模式2 的计划
	{	
//		switch (plan.status) {
//		case P1S_LIFT:
//			status_ctrl.expectedStatus.Pitch = Param.PFix;
//			status_ctrl.ghostExpect.Yaw = Param.YaGh;
//	
//			if (status.Altitude > 30 ) {
//				plan.status = P1S_FOLLOW_LINE;
//				Plan_StartTime();
//			}
//			break;

//		case P1S_FOLLOW_LINE:
//			status_ctrl.expectedStatus.Pitch = Param.PFix;
//			if (HyperCCD.run_out_of_line == 1 && plan.aux.mode2.out_of_line_counter < 4 && 
//					(plan.aux.mode2.out_of_line_counter == 0 || Plan_GetTime() > 2500) ) {
//				//出线，且次数小于4次	
//				plan.aux.mode2.out_of_line_counter ++; 
//				Plan_StartTime();
//				status_ctrl.expectedStatus.Yaw -=  95.0f;
//				status_ctrl.ghostExpect.Yaw = 95.0f;
//				plan.status = P1S_TURN_LEFT;
//				
//			} else if ((HyperCCD.mark_line || HyperCCD.run_out_of_line || Plan_GetTime() > 5000) && plan.aux.mode2.out_of_line_counter == 4 ) {
//				//扫描到标记线（粗线），且出线次数为4次
//				plan.status = P1S_RUN_OUT_OF_LINE ; 
//			} else {
//				plan_do_follow_line();
//			}
//			break;
//			
////		case P1S_TURN_LEFT_PRE:
////			status_ctrl.expectedStatus.Pitch = Param.PiBk;
//////			status_ctrl.expectedStatus.Roll  = Param.RFix - Param.YFix;
////			if (Plan_GetTime() > 500 || HyperCCD.run_out_of_line == 00) {
////				//Ready for Turn_Left
////				status_ctrl.expectedStatus.Yaw -=  90;
////				plan.status = P1S_TURN_LEFT;
////				Plan_StartTime();
////			}
////			break;
//			
//		case P1S_TURN_LEFT:
//			status_ctrl.expectedStatus.Pitch = Param.PiBk;
//			status_ctrl.expectedStatus.Roll  = Param.RFix - Param.YFix;
//			
//			status_ctrl.ghostExpect.Yaw -= Param.YaDn;
//			if (status_ctrl.ghostExpect.Yaw < Param.YaGh) {
//				status_ctrl.ghostExpect.Yaw = Param.YaGh; 
//			}
//		
//			if (
//				((fabsf(angleNorm2(status.Yaw - status_ctrl.expectedStatus.Yaw ))< 5) && 
//				(HyperCCD.run_out_of_line == 0 ))
//			) {
//				//恢复到出线前 
//				status_ctrl.ghostExpect.Yaw = Param.YaGh; 
//				plan.status = P1S_TURN_LEFT_POST;
//				status_ctrl.expectedStatus.Roll  = Param.RFix;
//				Plan_StartTime();
//			} 
//			break;
//			
//		case P1S_TURN_LEFT_POST:
//			status_ctrl.expectedStatus.Pitch = 
//				(plan.aux.mode2.out_of_line_counter == 2 || plan.aux.mode2.out_of_line_counter == 4) ? Param.PiG2 : Param.PiGo;
//			if (HyperCCD.run_out_of_line == 0)
//				plan_do_follow_line();
//			if (Plan_GetTime() > 500) {
//				plan.status = P1S_FOLLOW_LINE;
//				Plan_StartTime();
//			}
//			break;
//				
//		case P1S_RUN_OUT_OF_LINE:
//			stopflying();
//			break; 


//		}
switch (plan.status) {

		case P1S_LIFT:
			status_ctrl.expectedStatus.Pitch = Param.PFix;//稳住起飞；
			status_ctrl.ghostExpect.Yaw = Param.YaGh;
	
			if (status.Altitude > 30 ) {
				plan.status = P1S_FOLLOW_LINE;
				Plan_StartTime();
			}
			break;
			case P1S_FOLLOW_LINE:
			//status_ctrl.expectedStatus.Pitch = Param.PFix;
			if (HyperCCD.run_out_of_line == 1 && plan.aux.mode2.out_of_line_counter < 4 && 
					(plan.aux.mode2.out_of_line_counter == 0 || Plan_GetTime() > 2500) ) {
				//出线，且次数小于4次	
				plan.aux.mode2.out_of_line_counter ++; 
				Plan_StartTime();
				status_ctrl.expectedStatus.Yaw -=  90.0f;
				status_ctrl.ghostExpect.Yaw = 90.0f;
				plan.status = P1S_TURN_LEFT;
				
			} else if ((HyperCCD.mark_line || HyperCCD.run_out_of_line || Plan_GetTime() > 5000) && plan.aux.mode2.out_of_line_counter == 4 ) {
				//扫描到标记线（粗线），且出线次数为4次
				plan.status = P1S_RUN_OUT_OF_LINE ; 
			} else {
				plan_do_follow_line();
			}
			break;
     	case P1S_TURN_LEFT:
				if (plan.aux.mode2.out_of_line_counter ==1){
			status_ctrl.expectedStatus.Roll  = Param.RFix - Param.YFi1;
				}
       if (plan.aux.mode2.out_of_line_counter ==2){
			status_ctrl.expectedStatus.Roll  = Param.RFix - Param.YFi2;
				}
	    	if (plan.aux.mode2.out_of_line_counter ==3){
			status_ctrl.expectedStatus.Roll  = Param.RFix - Param.YFi3;
				}
	      if (plan.aux.mode2.out_of_line_counter ==4){
			status_ctrl.expectedStatus.Roll  = Param.RFix - Param.YFi4;
				}
			
			status_ctrl.ghostExpect.Yaw -= Param.YaDn;
			if (status_ctrl.ghostExpect.Yaw < Param.YaGh) {
				status_ctrl.ghostExpect.Yaw = Param.YaGh; 
			}
		
			if (
				((fabsf(angleNorm2(status.Yaw - status_ctrl.expectedStatus.Yaw ))< 30) && 
				(HyperCCD.run_out_of_line == 0 ))
			) {
				//恢复到出线前 
				status_ctrl.ghostExpect.Yaw = Param.YaGh; 
				plan.status = P1S_TURN_LEFT_POST;
				status_ctrl.expectedStatus.Roll  = Param.RFix;
				Plan_StartTime();
			} 
			break;
		case P1S_TURN_LEFT_POST:
			//status_ctrl.expectedStatus.Pitch = 
			//	(plan.aux.mode2.out_of_line_counter == 2 || plan.aux.mode2.out_of_line_counter == 4) ? Param.PiG2 : Param.PiGo;
		  if (plan.aux.mode2.out_of_line_counter ==1){
		   status_ctrl.expectedStatus.Pitch = Param.PiGo;
				}
       if (plan.aux.mode2.out_of_line_counter ==2){
		  	status_ctrl.expectedStatus.Roll  = Param.PiG1 ;
				}
	    	if (plan.aux.mode2.out_of_line_counter ==3){
			 status_ctrl.expectedStatus.Pitch = Param.PiG2 ;
				}
	      if (plan.aux.mode2.out_of_line_counter ==4){
		  status_ctrl.expectedStatus.Pitch = Param.PiG3;
				}
			if (HyperCCD.run_out_of_line == 0)
				plan_do_follow_line();
			if (Plan_GetTime() > 500) {
				plan.status = P1S_FOLLOW_LINE;
				Plan_StartTime();
			}
			break;
				
		case P1S_RUN_OUT_OF_LINE:
			stopflying();
			break; 

			
			


}

	}
	else if (Param.Mode == 3) // 模式3 的计划
	{
		switch (plan.status) {
		case P1S_LIFT:
			status_ctrl.expectedStatus.Pitch = Param.PFix;  //试试看吧
			if (status.Altitude > 20) {
				plan.status = P1S_FOLLOW_LINE;
			}
			break;
			
		case P1S_FOLLOW_LINE:
			status_ctrl.expectedStatus.Pitch = Param.PiGo;
			if (HyperCCD.run_out_of_line == 1 ) {
				//如果出线了
				if (plan.aux.mode3.is_backing == 0) {
					//转身
					plan.status = P1S_TURN_LEFT_PRE;
					Magnet_Put();
				} else {
					plan.status = P1S_RUN_OUT_OF_LINE;
				}
			} else {
				plan_do_follow_line();
			}
			break;
			
		case P1S_TURN_LEFT_PRE:
			status_ctrl.expectedStatus.Pitch = Param.PFix;
			status_ctrl.expectedStatus.Roll = Param.RFix - Param.YFix;
			if (Plan_GetTime() > 2000) {
				Plan_StartTime();
				plan.status = P1S_TURN_LEFT;
				status_ctrl.expectedStatus.Yaw += 180;
				status_ctrl.ghostExpect.Altitude = Param.SGGD;
			}
			break;
				
		case P1S_TURN_LEFT:
			status_ctrl.expectedStatus.Pitch = Param.PFix;
			status_ctrl.expectedStatus.Roll = Param.RFix;
			if (HyperCCD.run_out_of_line == 0) {
				plan_do_follow_line();
			}
			if (
				(
				(fabsf(angleNorm2(status.Yaw - status_ctrl.expectedStatus.Yaw )) < 5) || 
				(HyperCCD.run_out_of_line == 0 )
				) && 
				(Plan_GetTime() > 2000)
			) {
				//恢复到出线前
				Plan_StartTime();
				status_ctrl.ghostExpect.Altitude = 0.0f;
				plan.aux.mode3.is_backing = 1;
				plan.status = P1S_TURN_LEFT_POST;
			}
			break;
			
		case P1S_TURN_LEFT_POST:
			status_ctrl.expectedStatus.Pitch = Param.PiGo;
			if (HyperCCD.run_out_of_line == 0) {
				plan_do_follow_line();
			}
			if (Plan_GetTime() > 1000)
				plan.status = P1S_FOLLOW_LINE;
			break;
			
		case P1S_RUN_OUT_OF_LINE:
			stopflying();
			break;          //停止的问题再调。
					
		}
	} 
	else if(Param.Mode == 4) // 模式4 的计划
	{
	}

}



void Plan_StartTime(void)
{
	plan.time_since = millis();
}

uint32_t Plan_GetTime(void)
{
	return (millis() - plan.time_since);
}


void stopflying(void){
	
	Magnet_Idle();
	
	status_ctrl.expectedStatus.Pitch -= Param.FAA1;
	
	if (Flight_Working == FWS_FLYING)
		Flight_Working = FWS_LANDING;
	else
		Flight_Working = FWS_IDLE;
	plan.isWorking = 0;
	
	Param.Tmot = 0;
	
}
