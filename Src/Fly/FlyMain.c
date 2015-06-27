#include "FlyBasic.h"
#include "FlyMain.h"
#include "Param.h"

#include "TNavigator.h"
#include "TMessager.h"
#include "TPilot.h"

#include "Hardware/XRotor.h"
#include "Hardware/IIC.h"
#include "Hardware/USART.h"

int16_t Motor_Out[4] = {0,0,0,0};
int16_t Thro = 50;
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
	if (GPIOA->IDR & 0x10) Mode1Main(); //PA4 High and goes debug mode 1
	if (GPIOA->IDR & 0x20) Mode2Main(); //PA4 High and goes debug mode 1
	
	//Load Threads
	Init_TPilot();
	Init_MessagerThread();
	Init_Navigator();
	
	//Coroutine the Threads
	while(1) {
		Do_TPilot();
		Do_MessagerThread();
		Do_Navigator();
	}
}

void Mode2Main(void) {
	
}

void Mode1Main(void) {
	Motor_Configure_Start();		//Enter X-Rotor Selector mode
	while(GPIOA->IDR & 0x10);	 	//wait until PA4 gets low
	Motor_Configure_Confrim();		//That's my skateboard!
	
	//Remeber to reboot now
	while(1);
}
