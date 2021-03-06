#ifndef ADNS3080_H
#define ADNS3080_H

#include "sys.h"

typedef struct {
	int _sumX;
	int _sumY;
	float H;	//高度 单位 mm 
	float sumX;
	float sumY;
} ADNS3080_Typedef;

extern volatile ADNS3080_Typedef ADNS3080;

void ADNS3080_Init(void);
void ADNS3080_Reset(void);

void ADNS3080_Burst_Read(void);

#endif
