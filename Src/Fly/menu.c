#include "sys.h"
#include "menu.h"
#include "Hardware/HyperCCD.h"
#include "Hardware/LED.h"
#include "Hardware/oled.h"
#include "DataPacker.h"

typedef struct _ParamItem
{
	char name[4];
	float value;
} ParamItem;

ParamItem table1[] = {
	{"Mode", 1},
	{"gye", 10},
	{"gnp", -0.43},
	{"gnd", -1},
	{"gni", -0.7},
	{"gno", 4},
	{"gne", 67},
	{"RFix", 7.7},
	{"YFix", 5},
	{"PFix", 3},
	{"PiGo", 2},
	{"FAA1", 16},
	{"gpp", -0.11},
	{"gpi", -0.22},
	{"gpd", -0.07},
	{"gpe", 2},
	{"gpl", 4},
	{"gpo", 30},
	{"grp", 0.18},
	{"gri", 0.24},
	{"grd", 0.1},
	{"gre", 6.5},
	{"grl", 4},
	{"gro", 30},
	{"gyp", 0.50},
	{"gyi", 0.35},
	{"gyd", -0.50},
	{"gap", 0.3},
	{"gai", 0.26},
	{"gad", 0.5},
	{"gae", 60},
	{"gao", 30},
	{"gal", 20},
	{"ThUp", 0.2},
	{"ThDn", 0.1},
	{"Thro", 54},
	{"Work", 1},
};

ParamItem table2[] = {
	{"Mode", 2},
	{"gye", 0},
	{"gnp", -0.07},
	{"gnd", -0.07},
	{"gni", -0.06},
	{"gno", 10},
	{"gne", 67},
	{"RFix", 0.2},
	{"YaDn", 8},
	{"YaGh", 10},
	{"PFix", 6.5},
	{"PiG3", 12},
	{"PiG2", 11},
	{"PiG1", 12},
	{"PiGo", 8},
	{"YFi1", 3},
	{"YFi2", 3},
	{"YFi3", 3},
	{"YFi4", 2},
	{"FAA1", 16},
	{"gpp", -0.15},
	{"gpi", -0.22},
	{"gpd", -0.07},
	{"gpe", 6},
	{"gpl", 4},
	{"gpo", 30},
	{"grp", 0.18},
	{"gri", 0.26},
	{"grd", 0.16},
	{"gre", 0},
	{"grl", 4},
	{"gro", 30},
	{"gyp", 0.50},
	{"gyi", 0.5},
	{"gyd", -0.50},
	{"gap", 0.3},
	{"gai", 0.1},
	{"gad", 0.33},
	{"gae", 50},
	{"gao", 30},
	{"gal", 20},
	{"ThUp", 0.1},
	{"ThDn", 0.26},
	{"Thro", 48},
	{"Work", 1},
};

ParamItem table3[] = {
	{"Mode",  3},
	{"gye",  10},
	{"gnp",  -0.43},
	{"gnd",  -1},
	{"gni",  -0.7},
	{"gno",  4},
	{"gne",  67},
	{"RFix",  0},
	{"YFix",  2},
	{"FAA1",  4},
	{"PFix",  11},
	{"PiGo",  14},
	{"PiG2",  13},
	{"PiBk",  -9},
	{"SGGD",  0},
	{"gpp",  -0.11},
	{"gpi",  -0.28},
	{"gpd",  -0.07},
	{"gpe",  13},
	{"gpl",  4},
	{"gpo",  30},
	{"grp",  0.18},
	{"gri",  0.24},
	{"grd",  0.1},
	{"gre",  0},
	{"grl",  4},
	{"gro",  30},
	{"gyp",  0.50},
	{"gyi",  0.35},
	{"gyd",  -0.50},
	{"gap",  0.28},
	{"gai",  0.26},
	{"gad",  0.46},
	{"gae",  50},
	{"gao",  30},
	{"gal",  20},
	{"ThUp",  0.14},
	{"ThDn",  0.14},
	{"Thro",  56},
	{"Work",  1},
};

ParamItem table4[] = {
	{"Mode",  4},
	{"gye",  10},
	{"gnp",  -0.43},
	{"gnd",  -1},
	{"gni",  -0.7},
	{"gno",  4},
	{"gne",  67},
	{"RFix",  0},
	{"YFix",  2},
	{"FAA1",  4},
	{"PFix",  11},
	{"PiGo",  14},
	{"PiG2",  13},
	{"PiBk",  -9},
	{"SGGD",  0},
	{"gpp",  -0.11},
	{"gpi",  -0.28},
	{"gpd",  -0.07},
	{"gpe",  13},
	{"gpl",  4},
	{"gpo",  30},
	{"grp",  0.18},
	{"gri",  0.24},
	{"grd",  0.1},
	{"gre",  0},
	{"grl",  4},
	{"gro",  30},
	{"gyp",  0.50},
	{"gyi",  0.35},
	{"gyd",  -0.50},
	{"gap",  0.28},
	{"gai",  0.26},
	{"gad",  0.46},
	{"gae",  50},
	{"gao",  30},
	{"gal",  20},
	{"ThUp",  0.14},
	{"ThDn",  0.14},
	{"Thro",  56},
	{"Work",  1},
};


ParamItem *tablePtr;
uint8_t tableID = 1;
uint8_t tableLen;

static uint8_t ScanKey(void) {
	if (!ESP_Read(Pin_KEY_1)) return 1;
	if (!ESP_Read(Pin_KEY_2)) return 2;
	if (!ESP_Read(Pin_KEY_3)) return 3;
	return 0;
}

void MENU_Init(void)
{
	uint8_t i, key;
	OLED_Init();
	
	GPIOA->PUPDR &= ~0x7e00;
	GPIOA->PUPDR |=  0x2a00;
	delay_ms(50);
	
	OLED_CLS();
	INV_OLED_P6x8Str(0,0," * NUEDC 2015 Copter ");
	
	while(1) {
		key = ScanKey();
		
		OLED_P6x8Printf(0,1," Confirm Table:");
		OLED_write_1_num(20,2,tableID);
		
		if (key == 1) {	//K1 : ADD 
			break;
		} else if (key) { //confirm
			if (++tableID == 5) 
				tableID = 1;
		}
		
		while(ScanKey());
		delay_ms(200);
	}
	
	
	////////////////////////////// GO
	
	switch (tableID) {
		case 1:
			tablePtr = table1;
			tableLen = sizeof(table1);
			break;
		case 2:
			tablePtr = table2;
			tableLen = sizeof(table2);
			break;
		case 3:
			tablePtr = table3;
			tableLen = sizeof(table3);
			break;
		case 4:
			tablePtr = table4;
			tableLen = sizeof(table4);
			break;
	}
	
	OLED_Fill(0xFF);
	OLED_P6x8PrintfINV(20,4,"%d par by #%d", (int)tableLen, (int)tableID);
	for (i=5;i;i--) {
		LED_ON(1);
		LED_ON(2);
		LED_ON(3);
		LED_ON(4);
		INV_OLED_P6x8Str(20,2,"       ");
		delay_ms(200);
		
		LED_OFF(1);
		LED_OFF(2);
		LED_OFF(3);
		LED_OFF(4);
		INV_OLED_P6x8Str(20,2,"WARNING");
		delay_ms(200);
	}
}

void MENU_Summon(void)
{
	uint8_t ii;
	volatile union {
		char c[4];
		float v;
	} tmp1;
	
	OLED_Fill(0);
	OLED_P6x8Printf(20,4,"%d par by #%d", (int)tableLen, (int)tableID);
	
	DP_Feed(0xFE);
	DP_Feed(0xFC);
	DP_Feed(tableLen);
	for (ii=0;ii<tableLen;ii++) {
		DP_Feed(tablePtr->name[0]);
		DP_Feed(tablePtr->name[1]);
		DP_Feed(tablePtr->name[2]);
		DP_Feed(tablePtr->name[3]);
		
		tmp1.v = tablePtr->value;
		DP_Feed(tmp1.c[0]);
		DP_Feed(tmp1.c[1]);
		DP_Feed(tmp1.c[2]);
		DP_Feed(tmp1.c[3]);
		
		tablePtr++;
	}
}
