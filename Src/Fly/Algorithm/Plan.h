#ifndef __PLAN_H__
#define __PLAN_H__

#include "sys.h"
#include "StatusCtrl.h"

typedef struct {
	struct {
		uint32_t timeMax;	// 与前一个点的最大时间差
		float alt;			// 高度
		float aux1,aux2;
	} cond;	//Condition
	
	Status_Typedef expect;
	
} Waypoint_t;

typedef struct {
	Waypoint_t points[128];
	uint16_t currentPoint;
	uint16_t pointCount;
	uint32_t time_since;
	
	uint8_t isWorking;
} Plan_t;

extern Plan_t plan;

uint32_t Plan_GetTime(void);
void Plan_Init(void);
void Plan_Process(void);

#endif
