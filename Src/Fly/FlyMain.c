#include "FlyBasic.h"
#include "FlyMain.h"
#include "Param.h"

#include "TNavigator.h"
#include "TMessager.h"
#include "TPilot.h"
#include "TCCD.h"

#include "Hardware/XRotor.h"
#include "Hardware/IIC.h"
#include "Hardware/USART.h"

char Flight_Working = 0;

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
	
	//Special Modes
	if (!ESP_Read(Pin_MODE1)) Mode1Main();	//Key1 and goes debug mode 1
	if (!ESP_Read(Pin_MODE2)) Mode2Main();	//Key2 and goes debug mode 2
	
	//Load Threads
	Init_TPilot();
	Init_MessagerThread();
	Init_Navigator();
	Init_TCCD();
	
	//Coroutine the Threads
	while(1) {
		Do_TPilot();
		Do_MessagerThread();
		Do_Navigator();
		Do_TCCD();
	}
}

void Mode2Main(void) {
	
}

void Mode1Main(void) {
	Motor_Configure_Start();		//Enter X-Rotor Selector mode
	while(!ESP_Read(Pin_MODE1));	 	//wait until PA4 gets low
	Motor_Configure_Confrim();		//That's my skateboard!
	
	//Remeber to reboot now
	while(1);
}
