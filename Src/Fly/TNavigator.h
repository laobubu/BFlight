#ifndef TNavigator_H_
#define TNavigator_H_

#include "FlyBasic.h"

extern struct pt ptNavigator;
extern unsigned char nav_position;
extern u8 run_out_of_line;

void Init_Navigator(void);
void   Do_Navigator(void);
void  nav_handle(void);

#endif
