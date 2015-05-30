#ifndef __CCD_H__
#define __CCD_H__

#include "FlyBasic.h"

// 定义了 CCD 对象以及控制函数

typedef struct {
	uint8_t data[128];
} CCD_TypeDef;

extern CCD_TypeDef CCD;

void CCD_Init(void);		//初始化
void CCD_Sample(void);		//开始采样

#endif
