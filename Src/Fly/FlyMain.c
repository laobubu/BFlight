#include "FlyBasic.h"
#include "FlyMain.h"
#include "Param.h"

#include "menu.h"

#include "TNavigator.h"
#include "TMessager.h"
#include "TPilot.h"
#include "TCCD.h"

#include "Hardware/LED.h"
#include "Hardware/XRotor.h"
#include "Hardware/IIC.h"
#include "Hardware/USART.h"

Flight_Working_Status Flight_Working = FWS_IDLE;

void Mode1Main(void);
void Mode2Main(void);
void FlyBasic_Init(void);

void FlyMain(void) {
	//Initial Hardware
	Param_Reset();
	//patchedPrintf_Init();		//for FreeRTOS
	FlyBasic_Init();
	IICinit();
	Motor_Init();
	
	LED_ON(2);
	LED_ON(3);
	LED_ON(4);
	
	GPIOA->PUPDR &= ~0x7e00;
	GPIOA->PUPDR |=  0x2a00;
	
	delay_ms(100);
	
//	//Special Modes
//	if (!ESP_Read(Pin_MODE1)) Mode1Main();	//Key1 and goes debug mode 1
//	if (!ESP_Read(Pin_MODE2)) Mode2Main();	//Key2 and goes debug mode 2
//	
	//For Secure
	Motor_SetAllSpeed(0,0,0,0);
	
	//Load Threads
	Init_TPilot();
	Init_Navigator();
	//#ifdef MENU_MODE
	if (ESP_Read(Pin_KEY_1)) 
		Init_MessagerThread(0);
	else
		Init_MessagerThread(1);
	//Init_TCCD();
	
	if (ESP_Read(Pin_KEY_1)) 
	MENU_Init();
	
	//Coroutine the Threads
	while(1) {
		Do_TPilot();
		Do_MessagerThread();
		Do_Navigator();
		//Do_TCCD();
	}
}

void Mode2Main(void) {
	
}

void Mode1Main(void) {
//	Motor_Configure_Start();		//Enter X-Rotor Selector mode
//	while(!ESP_Read(Pin_MODE1));	 	//wait until PA4 gets low
//	Motor_Configure_Confrim();		//That's my skateboard!
//	WARNING! THIS IS DANGEROUS! DO NOT UNCOMMENT IF YOU KNOW WHAT THIS DOES.
	
	//Remeber to reboot now
	while(1) {
		LED_OFF(2);
		delay_ms(500);
		LED_ON(2);
		delay_ms(500);
	}
}
