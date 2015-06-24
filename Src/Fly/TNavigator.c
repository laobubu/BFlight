//导航程序，负责修改PID的期望什么的

#include "TNavigator.h"
#include "FlyBasic.h"
#include "Algorithm/PID.h"

struct pt ptNavigator;
PT_THREAD(TNavigator(struct pt *pt));

void Init_Navigator(void) {
	PT_INIT(&ptNavigator);
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
		
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
