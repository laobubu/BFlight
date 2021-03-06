#ifndef __PLAN_H__
#define __PLAN_H__

#include "sys.h"
#include "StatusCtrl.h"



typedef enum {
	P1S_LIFT = 0,
	P1S_FOLLOW_LINE,
  P1S_TURN_LEFT_PRE,
	P1S_TURN_LEFT,
	P1S_TURN_LEFT_POST,
	P1S_RUN_OUT_OF_LINE,
	P1S_GET_LOWER,
} PLAN1_STATUS_TYPE;

typedef struct {
	PLAN1_STATUS_TYPE status;
	uint32_t time_since;
	uint8_t isWorking;
	
	struct {
		PID_Typedef follow_line;
		float follow_line_expect;
	} pid;
	
	union {
		struct {
			uint8_t out_of_line_counter;
		} mode2;
		struct {
			uint8_t is_backing:1;
		} mode3;
	} aux;
} Plan_t;

extern Plan_t plan;

void Plan_StartTime(void);
uint32_t Plan_GetTime(void);

void Plan_Init(void);
void Plan_Prestart(void);
void Plan_Start(void);
void Plan_Process(void);

#endif
