#include "sys.h"

#include "FlyBasic.h"
#include "AlgorithmBasic.h"
#include "Param.h"
#include "Plan.h"
#include "StatusCalc.h"
#include "Algorithm/PID.h"

#include "Hardware/HyperCCD.h"
#include "Hardware/ADNS3080.h"


Plan_t plan;

PID_Typedef pidRollE;
PID_Typedef pidYawE;
PID_Typedef pidFlowE; //ROLL的光流PID
PID_Typedef pidFlewE;// PITCH的光流PID 
float newx,newy,prex=0,prey=0,xv=0,yv=0,prexv=0,preyv=0;

float pidRollE_Expect = 73;
float pidYawE_Expect = 73;
float pidFlowE_Expect = 0;
float pidFlewE_Expect = 0;
int  left_flag = 0;
int  right_flag = 0 ;
int  back_flag = 0 ;


void stopflying(void);
void Flowfliter(int x,int y);

void Plan_Init(void) {
	PID_Init(&pidRollE , 		PID_MODE_DERIVATIV_CALC, 	0.005f);
  PID_Init(&pidYawE , 		PID_MODE_DERIVATIV_CALC, 	0.005f);
	PID_Init(&pidFlowE , 		PID_MODE_DERIVATIV_CALC, 	0.005f);
	PID_Init(&pidFlewE , 		PID_MODE_DERIVATIV_CALC, 	0.005f);
}

void Plan_Start(void) {
	plan.time_since = millis();
	plan.isWorking = 1;
	plan.status = (PLAN1_STATUS_TYPE)0;
	ADNS3080.sumX = 0;
	ADNS3080.sumY = 0;
	ADNS3080._sumX = 0;
	ADNS3080._sumY = 0;
}

void Plan_Process(void) {
	//如果现在不执行计划，则退出该函数
		if (!plan.isWorking) return;
//在下面写计划就好了；



if (Param.Mode == 1)	// 模式1 的计划 
{
	switch (plan.status) {
		case P1S_LIFT:
			//status_ctrl.expectedStatus.Altitude = 40;
			if (status.Altitude > 30) {
				plan.status = P1S_FOLLOW_LINE;
			}
			break;
			
		case P1S_FOLLOW_LINE:
			if (HyperCCD.run_out_of_line == 1 ){
				plan.status = P1S_RUN_OUT_OF_LINE;
			}
				if (HyperCCD_HasNewData()) {
					status_ctrl.expectedStatus.Roll = Param.RFix + PID_Postion_Cal(
						&pidRollE,
						pidRollE_Expect,
						HyperCCD.nav_position,
						0,
						HyperCCD.time
					);
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
			//status_ctrl.expectedStatus.Altitude = 40;
			if (status.Altitude > 30) {
				plan.status = P1S_FOLLOW_LINE;
			}
			break;
			case P1S_FOLLOW_LINE:
			if (HyperCCD.run_out_of_line == 1 ){
				plan.status = P1S_TURN_LEFT;
				left_flag  = 1 ; 
			}
			if(HyperCCD.mark_line == 2){
			  plan.status = P1S_RUN_OUT_OF_LINE ; 
			}
		   if (HyperCCD_HasNewData()) {
					status_ctrl.expectedStatus.Roll = Param.RFix + PID_Postion_Cal(
						&pidRollE,  
						pidRollE_Expect,
						HyperCCD.nav_position,
						0,
						HyperCCD.time
					);
				}
			
		 break;
  	case P1S_TURN_LEFT:
			if(left_flag == 1){
			status_ctrl.expectedStatus.Roll -= Param.YFix ; 
			status_ctrl.expectedStatus.Yaw -=  90;
			left_flag  = 0 ; 
			}
	 if ((fabsf(status.Yaw - status_ctrl.expectedStatus.Yaw )< 5)||(HyperCCD.run_out_of_line == 0 ) ){
		 status_ctrl.expectedStatus.Roll += Param.YFix ; 
		 plan.status = P1S_FOLLOW_LINE;
	     } 
		break;
  	case P1S_RUN_OUT_OF_LINE:
       stopflying();
		break; 
			
			
		}
			
}	else if(Param.Mode == 3)// 模式3 的计划
{
  switch (plan.status) {
		case P1S_LIFT:
			//status_ctrl.expectedStatus.Altitude = 40;
			if (status.Altitude > 30) {
				plan.status = P1S_FOLLOW_LINE;
			}
			break;
			
		case P1S_FOLLOW_LINE:
		if(back_flag) {
			if (HyperCCD.run_out_of_line == 1 ){
		  plan.status = P1S_RUN_OUT_OF_LINE;
			}
		}
		if(!back_flag){
			if (HyperCCD.run_out_of_line == 1 ){
				plan.status = P1S_TURN_BACK;
				back_flag  = 1;
			}
			}
			if (HyperCCD.mark_line  == 1){
			   plan.status = P1S_HIGH ;
			}
				if (HyperCCD_HasNewData()) {
					status_ctrl.expectedStatus.Roll = Param.RFix + PID_Postion_Cal(
						&pidRollE,
						pidRollE_Expect,
						HyperCCD.nav_position,
						0,
						HyperCCD.time
					);
				}
			
		 break;
    case P1S_TURN_BACK:
			  status_ctrl.expectedStatus.Yaw += 180;
			  plan.status = P1S_FOLLOW_LINE;
		break;
		case P1S_HIGH:
			status_ctrl.expectedStatus.Altitude = 80;
			plan.status = P1S_FOLLOW_LINE;
		break ;
		case P1S_RUN_OUT_OF_LINE:
       stopflying();
		break;          //停止的问题再调。
				
	}  ////以上是巡线程序；
	
	
} else if(Param.Mode == 4) // 模式4 的计划
{
switch (plan.status) {
		case P1S_LIFT:
			//status_ctrl.expectedStatus.Altitude = 40;
			if (status.Altitude > 30) {
				plan.status = P1S_FOLLOW_LINE;
			}
			break;
			
		case P1S_FOLLOW_LINE:
		if (back_flag){
		   if (HyperCCD.run_out_of_line == 1 ){
				  plan.status = P1S_RUN_OUT_OF_LINE;
			}
		
		}
		if(!back_flag) {
			if (HyperCCD.run_out_of_line == 1 ){
				plan.status = P1S_SURF;
				back_flag = 1;
			}
		}
				if (HyperCCD_HasNewData()) {
					status_ctrl.expectedStatus.Roll = Param.RFix + PID_Postion_Cal(
						&pidRollE,
						pidRollE_Expect,
						HyperCCD.nav_position,
						0,
						HyperCCD.time
					);
				}
			
		 break;
		case P1S_SURF:
				status_ctrl.expectedStatus.Yaw += 180;
			  plan.status = P1S_FOLLOW_LINE;
		
		break;

		case P1S_RUN_OUT_OF_LINE:
       stopflying();
		break;          //停止的问题再调。
				
	}  ////以上是巡线程序；
}

}
	
	



uint32_t Plan_GetTime(void)
{
	return (millis() - plan.time_since);
}


void stopflying(void){

	Flight_Working = FWS_IDLE;
	plan.isWorking = 0;
	back_flag = 0 ;
	plan.status = P1S_LIFT ;
	
}

void Flowfliter(int x,int y){
    newx = x/10;
    newy = y/10;	 //新值输入
	  xv = newx - prex ; 
	  yv = newy - prey ; //新速度计算
	if ((xv> 5)||(xv<-5)){
	    newx = prex + prexv ;
	}else{
	     prexv  = xv ; 
	}                   //速度异常时，新值替换速度不变；正常时，速度更新；
 
		if ((yv> 5)||(yv<-5)){
	    newy = prey + preyv ;
	}else{
	     preyv  = yv ; 
	}
	 prey = newy ;  
	 prex = newx ; 
}
