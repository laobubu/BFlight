//CCD程序

#include "TCCD.h"

#include "Hardware/CCD.h"

struct pt pTCCD;
PT_THREAD(TCCD(struct pt *pt));

uint16_t CCD_ExposeTime = 5;

void Init_TCCD(void) {
	PT_INIT(&pTCCD);
	
	CCD_Init();
}

void Do_TCCD(void) {
	TCCD(&pTCCD);
}

PT_THREAD(TCCD(struct pt *pt)) {
	PT_BEGIN(pt);
	while(1) {
		PT_TIMER_INTERVAL(pt, CCD_ExposeTime);  //CCD曝光时间
		
		CCD_Sample();
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
