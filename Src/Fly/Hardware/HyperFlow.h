#ifndef __HyperFlow_H_
#define __HyperFlow_H_

#include "sys.h"
#include "pin_map.h"

//超级CCD结构体
typedef struct {

  int x;
	int y;

} HyperFlow_t;

extern HyperFlow_t HyperFlow;

void HyperFlow_Init(void);
void HyperFlow_Feed(uint8_t ch);
uint8_t HyperFlow_HasNewData(void);

#endif
