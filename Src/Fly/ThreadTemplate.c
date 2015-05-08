#include "TExampleThread.h"

struct pt ptExampleThread;
PT_THREAD(TExampleThread(struct pt *pt));

void Init_ExampleThread(void) {
	PT_INIT(&ptExampleThread);
	
	//Add your init code here
	
}

void Do_ExampleThread(void) {
	TExampleThread(&ptExampleThread);
}

PT_THREAD(TExampleThread(struct pt *pt)) {

	//Add [static] variables here
	
	PT_BEGIN(pt);
	
	while(1) {
		// User code BEGIN
		
		PT_TIMER_DELAY(pt, 1000);	//Delay
		PT_WAIT_UNTIL(pt, 1==1);	//Wait until (this will block thread) ...
		PT_YIELD_UNTIL(pt, 1==1);	//Wait until (if Condition is false, system runs other threads) ...
		
		// User code END
		PT_YIELD(pt);				//Important! Keep this to make the system work!
	}
	
	PT_END(pt);
}
