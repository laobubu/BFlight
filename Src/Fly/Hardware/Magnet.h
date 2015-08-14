#ifndef __MAGNET_H_
#define __MAGNET_H_

#include "sys.h"
#include "pin_map.h"

void Magnet_Init(void);

#define Magnet_Get() 	do{ GPIOF->BSRR = 0x08;	    }while(0)		//PF3
#define Magnet_Put() 	do{	GPIOF->BSRR = 0x080000; }while(0)	
#define Magnet_Idle() do{	GPIOF->BSRR = 0x080000; }while(0)

#endif
