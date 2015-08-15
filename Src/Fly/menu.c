#include "sys.h"
#include "menu.h"
#include "Hardware/HyperCCD.h"
#include "Hardware/oled.h"

static uint8_t ScanKey(void) {
	//if (ESP_Read(Pin_KEY_1)
	return 0;
}

void MENU_Init(void)
{
	uint8_t i;
	OLED_Init();
	
	delay_ms(50);
	
	OLED_CLS();
	INV_OLED_P6x8Str(0,0," * NUEDC 2015 Copter ");
	
	while(1);
}
