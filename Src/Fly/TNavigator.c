//导航程序，负责修改PID的期望什么的

#include "TNavigator.h"
#include "FlyBasic.h"
#include "Param.h"

#include "Algorithm/PID.h"
#include "Algorithm/StatusCtrl.h"

//#include "Hardware/PX4Flow.h"
#include "Hardware/Laser.h"
#include "Hardware/LED.h"
#include "Hardware/CCD.h"

PID_Typedef  nav_pid;
typedef struct {
	float alt;
	float roll;
	float yaw;
	float pitch;
	int   run_out_line;
	int   obstacle_avoidance;
	int   stop_fly;
	int   turninng;
	int   find_home;
	int   find_target;
} Waypoint_t;

Waypoint_t plan[20];

unsigned char nav_position = 60;
u8 run_out_of_line = 0;

struct pt ptNavigator;
PT_THREAD(TNavigator(struct pt *pt));

void Init_Navigator(void) {
	PT_INIT(&ptNavigator);
	
	PID_Init(&nav_pid, PID_MODE_DERIVATIV_CALC, 1.0f);
	Laser_Init();
	//PX4Flow_Init();
}

void Do_Navigator(void) {
	TNavigator(&ptNavigator);
}

enum {
	BSET_NONE,
	BSET_LEFT,
	BSET_RIGHT
} bset = BSET_NONE;

u8 LED_is_Lighting = 0;

PT_THREAD(TNavigator(struct pt *pt)) {
	PT_BEGIN(pt);
	while(1) {
		PT_TIMER_INTERVAL(pt, 10);  //控制程序频率为 100 Hz
		
		LED_is_Lighting ^= 1;
		if (LED_is_Lighting) {
			LED_ON(1);
		} else {
			LED_OFF(1);
		}
		   
		//下面是CCD处理程序
		nav_handle();       		//差分处理
		status_ctrl.expectedStatus.Roll = PID_Postion_Cal(&nav_pid, 60, nav_position, 0, 0);
  //  status_ctrl.expectedStatus.Altitude = altplan[1];		
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
//		switch (bset) {
//			case BSET_NONE:
//				if (Laser_Read(LASER_LEFT))			bset = BSET_LEFT;
//				else if (Laser_Read(LASER_RIGHT))	bset = BSET_RIGHT;
//				break;
//			case BSET_LEFT:
//				if (!Laser_Read(LASER_LEFT)) {
//					if (Laser_Read(LASER_RIGHT))	bset = BSET_RIGHT;
//					else							bset = BSET_NONE;
//				}
//				break;
//			case BSET_RIGHT:
//				if (!Laser_Read(LASER_RIGHT)) {
//					if (Laser_Read(LASER_LEFT))		bset = BSET_LEFT;
//					else							bset = BSET_NONE;
//				}
//				break;
//		}
//		switch (bset) {
//			case BSET_NONE:
//				//ExpectedAngle[0] = Param.RFix;
//				break;
//			case BSET_LEFT:
//				//ExpectedAngle[0] = Param.RFix - Param.BOff;
//				break;
//			case BSET_RIGHT:
//				//ExpectedAngle[0] = Param.RFix + Param.BOff;
//				break;
//		}    //上面是避障的程序；
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
void  nav_handle(){
	unsigned char i, pmax = 255, pmin = 255;
	signed short dmax = 0, dmin = 0, dtmp = 0;
	
	for(i=20;i<100;i++){
		dtmp = CCD.data[i] - CCD.data[i+1];
					if (dtmp > dmax) {
						dmax = dtmp;
						pmax = i;
					}
					if (dtmp < dmin) {
						dmin = dtmp;
						pmin = i;
					}
		CCD.data[i] = dtmp/2 + 127;
	}
	
	if (dmax - dmin > 15)	{
				nav_position = (pmax + pmin)/2;
				CCD.data[nav_position] = 0xFF;
		    run_out_of_line = 0;
  }
	else {
	      run_out_of_line = 1;
	   
	}
	
}
	

