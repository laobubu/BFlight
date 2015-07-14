#ifndef TCCD_H_
#define TCCD_H_

#include "FlyBasic.h"

extern struct pt pTCCD;
extern uint16_t CCD_ExposeTime;

void Init_TCCD(void);
void   Do_TCCD(void);

#endif
