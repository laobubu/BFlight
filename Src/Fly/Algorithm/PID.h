#ifndef __PID_H_
#define __PID_H_

// PID结构体
typedef struct
{
    float P;
    float I;
    float D;
    float Desired;
	
    float Error;
    float PreError;
	
    float Integ;
	float iLimit;
    float Deriv;
	
    float Output;
 
}PID_Typedef;

void PID_Init(PID_Typedef * PID);
void PID_Postion_Cal(PID_Typedef *PID,float target,float measure, float dertT);
void PID_Limiter(float *data, float  range);

#endif
