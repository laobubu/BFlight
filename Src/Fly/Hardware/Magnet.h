#ifndef __LED_H_
#define __LED_H_

#include "sys.h"
#include "pin_map.h"

void Magnet_Init(void);

#define Magnet_Get() 	do{TIM9->CCR2=0;	TIM9->CCR1=100;	}while(0)
#define Magnet_Put() 	do{TIM9->CCR1=0;	TIM9->CCR2=100;	}while(0)
#define Magnet_Idle() 	do{TIM9->CCR1=0;	TIM9->CCR2=0;	}while(0)

#endif
