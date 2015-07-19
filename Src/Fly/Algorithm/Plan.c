#include "sys.h"
#include "AlgorithmBasic.h"
#include "Plan.h"
#include "Algorithm/PID.h"

Plan_t plan;

PID_Typedef pidRollE;
float pidRollE_Expect;

void Plan_Init(void) {
	plan.currentPoint = 0;
	plan.pointCount = 0;
	plan.time_since = millis();
	PID_Init(&pidRollE , 		PID_MODE_DERIVATIV_CALC, 	0.005f);
}

#define CURRENT (plan.points[plan.currentPoint])
#define NEXT    (plan.points[plan.currentPoint + 1])
void Plan_Process(void) {
//	
//	status_ctrl.expectedStatus.Pitch = 3;
//	status_ctrl.expectedStatus.Roll = PID_Postion_Cal(&pidRollE,pidRollE_Expect,position,0,dt);
//  status_ctrl.expectedStatus.Altitude = 30; 
//  status_ctrl.expectedStatus.Yaw = 30;	//大概就是这么写plan
}
