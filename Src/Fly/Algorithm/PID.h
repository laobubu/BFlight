#ifndef __PID_H_
#define __PID_H_

#include <stdint.h>

// PID结构体
typedef struct
{
    float P;
    float I;
    float D;
    float Desired;
    float Error;
    float PreError;
    float PrePreError;
    float Increment;
    float Integ;
	float iLimit;
    float Deriv;
    float Output;
 
}PID_Typedef;

void PID_Init(PID_Typedef * PID);
void PID_Postion_Cal(PID_Typedef *PID,float target,float measure, int dertT);

//以下两个方法是和整个系统耦合的
void PID_Init_All(void); 
void PID_Calc_All(void) ;
void PID_Limiter(float *data, float  range);

extern PID_Typedef pitch_PID;
extern PID_Typedef roll_PID;
extern PID_Typedef yaw_PID;
extern PID_Typedef alt_PID;

#endif
