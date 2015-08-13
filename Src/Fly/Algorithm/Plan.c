#include "sys.h"
#include "string.h"

#include "FlyBasic.h"
#include "AlgorithmBasic.h"
#include "Param.h"
#include "Plan.h"
#include "StatusCalc.h"
#include "Algorithm/PID.h"

#include "Hardware/HyperCCD.h"
#include "Hardware/ADNS3080.h"


Plan_t plan;


void stopflying(void);

void Plan_Init(void) {
	PID_Init(&plan.pid.follow_line , 		PID_MODE_DERIVATIV_CALC, 	0.005f);
}

void Plan_Start(void) {
	Plan_StartTime();
	plan.isWorking = 1;
	plan.status = (PLAN1_STATUS_TYPE)0;
	
	pid_Reset_Integral(&plan.pid.follow_line);
	
	memset(&plan.aux, 0, sizeof(plan.aux));
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



	if (Param.Mode == 1)	// 模式1 的计划 
	{
		switch (plan.status) {
			case P1S_LIFT:
				if (status.Altitude > 20 && Plan_GetTime() > 0) {
					plan.status = P1S_FOLLOW_LINE;
				}
				break;
				
			case P1S_FOLLOW_LINE:
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
		switch (plan.status) {
		case P1S_LIFT:
			status_ctrl.expectedStatus.Pitch = Param.PFix;
			if (status.Altitude > 30 ) {
				plan.status = P1S_FOLLOW_LINE;
			}
			break;

		case P1S_FOLLOW_LINE:
			status_ctrl.expectedStatus.Pitch = Param.PiGo;
			status_ctrl.expectedStatus.Roll  = Param.RFix;
			if (HyperCCD.run_out_of_line == 1 && plan.aux.mode2.out_of_line_counter < 4 ) {
				//出线，且次数小于4次
				plan.aux.mode2.out_of_line_counter ++;
				Plan_StartTime();
				plan.status = P1S_TURN_LEFT_PRE;
			} else if ((HyperCCD.mark_line || HyperCCD.run_out_of_line) && plan.aux.mode2.out_of_line_counter == 4 ) {
				//扫描到标记线（粗线），且出线次数为4次
				plan.status = P1S_RUN_OUT_OF_LINE ; 
			} else {
				plan_do_follow_line();
			}
			break;
			
		case P1S_TURN_LEFT_PRE:
			status_ctrl.expectedStatus.Pitch = Param.PiBk;
			status_ctrl.expectedStatus.Roll  = Param.RFix;
			if (Plan_GetTime() > 1500 || HyperCCD.run_out_of_line == 0) {
				//Ready for Turn_Left
				status_ctrl.expectedStatus.Yaw -=  90;
				plan.status = P1S_TURN_LEFT;
				Plan_StartTime();
			}				
			break;
			
		case P1S_TURN_LEFT:
			status_ctrl.expectedStatus.Pitch = Param.PFix - Param.YFix;
			status_ctrl.expectedStatus.Roll  = Param.RFix - Param.YFix;
			if (
				(fabsf(angleNorm2(status.Yaw - status_ctrl.expectedStatus.Yaw ))< 5) && 
				(HyperCCD.run_out_of_line == 0 ) &&
				(Plan_GetTime() > 1500)
			) {
				//恢复到出线前 
				plan.status = P1S_TURN_LEFT_POST;
				Plan_StartTime();
			} 
			break;
			
		case P1S_TURN_LEFT_POST:
			status_ctrl.expectedStatus.Pitch = Param.PiGo ;
			status_ctrl.expectedStatus.Roll  = Param.RFix + Param.YFix;
			plan_do_follow_line();
			if (Plan_GetTime() > 1500) {
				plan.status = P1S_FOLLOW_LINE;
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
			status_ctrl.expectedStatus.Pitch = Param.PiGo;  //试试看吧
			if (status.Altitude > 20) {
				plan.status = P1S_FOLLOW_LINE;
			}
			break;
			
		case P1S_FOLLOW_LINE:
			status_ctrl.expectedStatus.Pitch = Param.PFix;
			if (HyperCCD.run_out_of_line == 1 ) {
				//如果出线了
				if (plan.aux.mode3.is_backing == 0) {
					//转身
					status_ctrl.expectedStatus.Yaw += 180;
					plan.status = P1S_TURN_BACK;
				} else {
					plan.status = P1S_RUN_OUT_OF_LINE;
				}
			} else {
				plan_do_follow_line();
			}
			break;
				
		case P1S_TURN_BACK:
			status_ctrl.expectedStatus.Pitch = Param.PiGo;
			if (
				(fabsf(angleNorm2(status.Yaw - status_ctrl.expectedStatus.Yaw )) < 5) || 
				(HyperCCD.run_out_of_line == 0 )
			) {
				//恢复到出线前
				plan.aux.mode3.is_backing = 1;
				plan.status = P1S_FOLLOW_LINE;
			}
			break;
			
		case P1S_RUN_OUT_OF_LINE:
			stopflying();
			break;          //停止的问题再调。
					
		}
	} 
	else if(Param.Mode == 4) // 模式4 的计划
	{
		switch (plan.status) {
		case P1S_LIFT:
			if (status.Altitude > 30 && Plan_GetTime() > 3000) {
				plan.status = P1S_FOLLOW_LINE;
			}
			break;
			
		case P1S_FOLLOW_LINE:
			if (HyperCCD.run_out_of_line == 1 ) {
				//如果出线了
				if (plan.aux.mode3.is_backing == 0) {
					//转身
					status_ctrl.expectedStatus.Yaw += 180;
					plan.status = P1S_TURN_BACK;
				} else {
					plan.status = P1S_RUN_OUT_OF_LINE;
				}
			} else {
				plan_do_follow_line();
			}
			break;
				
		case P1S_TURN_BACK:
			if (
				(fabsf(angleNorm2(status.Yaw - status_ctrl.expectedStatus.Yaw )) < 5) || 
				(HyperCCD.run_out_of_line == 0 )
			) {
				//恢复到出线前
				plan.aux.mode3.is_backing = 1;
				plan.status = P1S_FOLLOW_LINE;
			}
			break;
			
		case P1S_RUN_OUT_OF_LINE:
			stopflying();
			break;          //停止的问题再调。

		}
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
	
	status_ctrl.expectedStatus.Pitch -= Param.FAA1;
	
	if (Flight_Working == FWS_FLYING)
		Flight_Working = FWS_LANDING;
	else
		Flight_Working = FWS_IDLE;
	plan.isWorking = 0;
	
}
