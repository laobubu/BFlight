#ifndef __HyperCCD_H_
#define __HyperCCD_H_

#include "sys.h"
#include "pin_map.h"

//超级CCD结构体
typedef struct {
	unsigned char nav_position;
	unsigned char time;
  int run_out_of_line:1;
  int mark_line:1;

} HyperCCD_t;

extern HyperCCD_t HyperCCD;

void HyperCCD_Init(void);
void HyperCCD_Feed(uint8_t ch);
uint8_t HyperCCD_HasNewData(void);

#endif
