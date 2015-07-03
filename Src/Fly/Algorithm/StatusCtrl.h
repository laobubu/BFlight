#ifndef __STATUS_CTRL_H__
#define __STATUS_CTRL_H__

#include "sys.h"
#include "StatusCalc.h"
#include "AlgorithmBasic.h"
#include "PID.h"

typedef struct {
	PID_Typedef PID_pitch;
	PID_Typedef PID_roll;
	PID_Typedef PID_yaw;
	PID_Typedef PID_alt;
	float PID_OutputLimit;
	
	Status_Typedef expectedStatus;
	
	int16_t Motor_Out[4];
	int16_t Thro;
} StatusCtrl_Typedef;

extern StatusCtrl_Typedef status_ctrl;

void SCx_Init(void);
void SCx_Process(void);

#endif
