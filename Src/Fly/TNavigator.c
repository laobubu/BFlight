//导航程序，负责修改PID的期望什么的

#include "TNavigator.h"
#include "FlyBasic.h"
#include "Param.h"

#include "Algorithm/PID.h"
#include "Hardware/PX4Flow.h"
#include "Hardware/Laser.h"

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
	
	Laser_Init();
	PX4Flow_Init();
}

void Do_Navigator(void) {
	TNavigator(&ptNavigator);
}

//float currentX, currentY;
#define currentX PX4Flow.x
#define currentY PX4Flow.y

enum {
	BSET_NONE,
	BSET_LEFT,
	BSET_RIGHT
} bset = BSET_NONE;

PT_THREAD(TNavigator(struct pt *pt)) {
	PT_BEGIN(pt);
	while(1) {
		PT_TIMER_INTERVAL(pt, 10);  //控制程序频率为 100 Hz
		
		//.................

//    PID_Postion_Cal(&X_PID,	ExpectedPos[0], currentX,	0xFFFFFF);
//    PID_Postion_Cal(&Y_PID,	ExpectedPos[1], currentY,	0xFFFFFF);
//		PID_Limiter(&X_PID.Output,5);
//		PID_Limiter(&Y_PID.Output,5);
//    ExpectedAngle[0] = -3 + X_PID.Output; 
//    ExpectedAngle[1] = -8 + Y_PID.Output; //x,y两轴PID控制
	
		/*
		if (roll_offset != 0.0 && Laser_Read(LASER_LEFT) && Laser_Read(LASER_RIGHT)) {
		} else {
			roll_offset = 0.0f;
			if (Laser_Read(LASER_LEFT))			roll_offset -= Param.BOff;
			else if (Laser_Read(LASER_RIGHT))	roll_offset += Param.BOff;
		}*/
		switch (bset) {
			case BSET_NONE:
				if (Laser_Read(LASER_LEFT))			bset = BSET_LEFT;
				else if (Laser_Read(LASER_RIGHT))	bset = BSET_RIGHT;
				break;
			case BSET_LEFT:
				if (!Laser_Read(LASER_LEFT)) {
					if (Laser_Read(LASER_RIGHT))	bset = BSET_RIGHT;
					else							bset = BSET_NONE;
				}
				break;
			case BSET_RIGHT:
				if (!Laser_Read(LASER_RIGHT)) {
					if (Laser_Read(LASER_LEFT))		bset = BSET_LEFT;
					else							bset = BSET_NONE;
				}
				break;
		}
		switch (bset) {
			case BSET_NONE:
				ExpectedAngle[0] = Param.RFix;
				break;
			case BSET_LEFT:
				ExpectedAngle[0] = Param.RFix - Param.BOff;
				break;
			case BSET_RIGHT:
				ExpectedAngle[0] = Param.RFix + Param.BOff;
				break;
		}
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
