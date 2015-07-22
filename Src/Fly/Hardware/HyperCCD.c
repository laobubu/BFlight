#include "FlyBasic.h"
#include "HyperCCD.h"

HyperCCD_t HyperCCD;

void HyperCCD_Init(void)
{
	//GPIO Init
}



//收到的数据

uint16_t HC_BytesLeft = 0;
uint8_t* HC_BBA;
HyperCCD_t HyperCCD_DataGot;

void HyperCCD_Feed(uint8_t ch)
{
	if (HC_BytesLeft == 0) {
		if (ch == 0xAA) {
			HC_BytesLeft = sizeof(HyperCCD_DataGot)-1;
		}
	} else if (HC_BytesLeft == (sizeof(HyperCCD_DataGot)-1)) {
		if (ch == 0xFF) {
			HC_BytesLeft--;
			HC_BBA = (uint8_t*) &HyperCCD_DataGot;
		} else {
			HC_BytesLeft = 0;
		}
	} else {
		*(HC_BBA++) = ch;
		if (!(--HC_BytesLeft)) {
			HyperCCD.nav_position = HyperCCD_DataGot.nav_position;
			HyperCCD.status = HyperCCD_DataGot.status;
			HyperCCD.time = HyperCCD_DataGot.time;
		}
	}
}
