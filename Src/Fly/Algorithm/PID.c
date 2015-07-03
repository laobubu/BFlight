#include "PID.h"
#include "AlgorithmBasic.h"
#include "string.h"

void PID_Limiter(float *data, float range) {
	if (*data >  range)  *data =  range;
	if (*data < -range)  *data = -range;
}

void PID_Init(PID_Typedef * PID) {
	memset(PID, 0, sizeof(PID_Typedef));
}

//-----------仅用于角度环和角速度环的位置式PID-----------
void PID_Postion_Cal(PID_Typedef * PID,float target,float measure, float delta)
{

	//-----------位置式PID-----------
	//误差=期望值-测量值
	PID->Error = target - measure;
	
	if (1e38 >= delta) {
		PID->Deriv = delta ;   // 第一层D直接用角速度，效果不错；
	} else {
		PID->Deriv = PID->Error - PID->PreError;//超声波等用传统的D
	}
	
	PID->Integ = PID->Integ + PID->Error;
	if (PID->iLimit) {
		if (PID->Error > PID->iLimit){
			PID->Integ = 0; 
		} else if (PID->Error < -PID->iLimit){
			PID->Integ = 0;                          //对I的修正为，当P大于一定值的时候不积分；
		}
	}
	 
	PID->Output = (PID->P * PID->Error) + (PID->I * PID->Integ) + (PID->D * PID->Deriv);
	PID->PreError = PID->Error;
}
