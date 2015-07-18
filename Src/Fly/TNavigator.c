//导航程序，负责修改PID的期望什么的

#include "TNavigator.h"
#include "FlyBasic.h"
#include "Param.h"

#include "Algorithm/PID.h"
#include "Algorithm/StatusCtrl.h"

#include "Hardware/LED.h"

PID_Typedef  nav_pid;

struct pt ptNavigator;
PT_THREAD(TNavigator(struct pt *pt));

void Init_Navigator(void) {
	PT_INIT(&ptNavigator);
}

void Do_Navigator(void) {
	TNavigator(&ptNavigator);
}


static u8 LED_is_Lighting = 0;

PT_THREAD(TNavigator(struct pt *pt)) {
	PT_BEGIN(pt);
	while(1) {
		PT_TIMER_INTERVAL(pt, 10);  //控制程序频率为 100 Hz
		
		//状态指示灯，只是用来看的而已
		LED_is_Lighting ^= 1;
		if (LED_is_Lighting) { LED_ON(1); } else {	LED_OFF(1); }
		
		//////////////////////////////////////////////////////////////
		/// Plan 自动控制流程 开始
		
		
		
		/// Plan 自动控制流程 结束
		//////////////////////////////////////////////////////////////
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
