#ifndef __MAGNET_H_
#define __MAGNET_H_

#include "sys.h"
#include "pin_map.h"

void Magnet_Init(void);

#define Magnet_Get() 	do{ GPIOF->BSRR = 0x040008; }while(0)		//PF3 + PF2
#define Magnet_Put() 	do{	GPIOF->BSRR = 0x080004; }while(0)	
#define Magnet_Idle() do{	GPIOF->BSRR = 0x0C0000; }while(0)

#endif
