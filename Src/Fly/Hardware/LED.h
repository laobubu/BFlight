#ifndef __LED_H_
#define __LED_H_

#include "sys.h"
#include "pin_map.h"

void LED_Init(void);

#define LED_ON(X)   ESP_Set(Pin_LED_##X);
#define LED_OFF(X)  ESP_Reset(Pin_LED_##X);

#endif
