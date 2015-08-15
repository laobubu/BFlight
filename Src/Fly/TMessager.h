#ifndef TMessager_H_
#define TMessager_H_

#include "FlyBasic.h"

extern struct pt ptMessagerThread;

void Init_MessagerThread(uint8_t enableHeartbeat);
void   Do_MessagerThread(void);

#endif
