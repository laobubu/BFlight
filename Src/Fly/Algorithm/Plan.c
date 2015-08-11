#include "sys.h"

#include "FlyBasic.h"
#include "AlgorithmBasic.h"
#include "Param.h"
#include "Plan.h"
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
int  flow_flag = 1 ;


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
	///////以下是光流的程序////////////


if (Param.Mode == 1)	// 模式1 的计划 
{
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
			if ((HyperCCD.turn_left== 1)&&(status.Altitude > 35) ){
				plan.status = P1S_TURN_LEFT;
				left_flag  = 1;
			}
		  if ((HyperCCD.turn_right== 1)&&((status.Altitude > 35) ) ){
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
			  status_ctrl.expectedStatus.Roll -= Param.YFix ; 
			  status_ctrl.expectedStatus.Yaw -= 90;
			   left_flag = 0;
			}

 if ((!HyperCCD.run_out_of_line)&&(fabsf(status.Yaw - status_ctrl.expectedStatus.Yaw )< 5)&&(!HyperCCD.turn_left)&&(!HyperCCD.turn_right)){
       status_ctrl.expectedStatus.Roll += Param.YFix ;          
	     plan.status = P1S_FOLLOW_LINE;
     } 
		break;
		case P1S_TURN_RIGHT:
			if(right_flag == 1){
			status_ctrl.expectedStatus.Roll += Param.YFix ; 
			status_ctrl.expectedStatus.Yaw +=  90;
			right_flag  = 0 ; 
			}
	 if ((fabsf(status.Yaw - status_ctrl.expectedStatus.Yaw )< 3) ){
		 status_ctrl.expectedStatus.Roll -= Param.YFix ; 
		 plan.status = P1S_FOLLOW_LINE;
	     } 
		break;
		//case P1S_RUN_OUT_OF_LINE:
      // stopflying();
		//break;          //停止的问题再议。
				
	}  ////以上是巡线程序；
	
} 
else if (Param.Mode == 2)	// 模式2 的计划
{	
	  ADNS3080.H = status.Altitude *10.0f;
		ADNS3080_Burst_Read();
	if(status.Altitude > 35){
		if ((pidFlowE_Expect - ADNS3080.sumX> 20)||(pidFlowE_Expect - ADNS3080.sumX < -20)||(pidFlewE_Expect - ADNS3080.sumY < -20)||(pidFlewE_Expect - ADNS3080.sumY < -20)){
		
		   flow_flag = 1;
		}
		
		if (flow_flag){
		    pidFlowE_Expect = ADNS3080.sumX;
			  pidFlewE_Expect = ADNS3080.sumY;
			  flow_flag = 0;
		}
		
	
		status_ctrl.expectedStatus.Roll = Param.RFix + PID_Postion_Cal(
						&pidFlowE,
						pidFlowE_Expect,
						ADNS3080.sumX,
						0,
						1
					);
		status_ctrl.expectedStatus.Pitch = Param.PFix + PID_Postion_Cal(
					&pidFlewE,
					pidFlewE_Expect,
				  ADNS3080.sumY,
				  0,
				  1
					);
	}

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
