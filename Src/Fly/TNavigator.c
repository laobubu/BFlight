//导航程序，负责修改PID的期望什么的

#include "TNavigator.h"
#include "FlyBasic.h"
#include "Algorithm/PID.h"

extern float ExpectedAngle[3];

PID_Typedef X_PID;
PID_Typedef Y_PID;
float ExpectedPos[2] = {0.,0.}; //X,Y


struct pt ptNavigator;
PT_THREAD(TNavigator(struct pt *pt));

void Init_Navigator(void) {
	PT_INIT(&ptNavigator);
	PID_Init(&X_PID);
	PID_Init(&Y_PID);
}

void Do_Navigator(void) {
	TNavigator(&ptNavigator);
}

float currentX, currentY;

PT_THREAD(TNavigator(struct pt *pt)) {
	PT_BEGIN(pt);
	while(1) {
		PT_TIMER_INTERVAL(pt, 20);  //控制程序频率为 50 Hz
		
		//.................

    PID_Postion_Cal(&X_PID,	ExpectedPos[0], currentX,	0xFFFFFF);
    PID_Postion_Cal(&Y_PID,	ExpectedPos[1], currentY,	0xFFFFFF);
		PID_Limiter(&X_PID.Output,5);
		PID_Limiter(&Y_PID.Output,5);
    ExpectedAngle[0] = -3 + X_PID.Output; 
    ExpectedAngle[1] = -8 + Y_PID.Output; //x,y两轴PID控制
	
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
