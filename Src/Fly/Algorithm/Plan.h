#ifndef __PLAN_H__
#define __PLAN_H__

#include "sys.h"
#include "StatusCtrl.h"

typedef enum {
	P1S_LIFT = 0,
	P1S_FOLLOW_LINE = 1,
	P1S_TURN_LEFT = 2,
	P1S_TURN_RIGHT = 3,
	P1S_RUN_OUT_OF_LINE = 4
} PLAN1_STATUS_TYPE;

typedef struct {
	PLAN1_STATUS_TYPE status;
	uint32_t time_since;
	uint8_t isWorking;
} Plan_t;

extern Plan_t plan;

uint32_t Plan_GetTime(void);
void Plan_Init(void);
void Plan_Start(void);
void Plan_Process(void);

#endif
