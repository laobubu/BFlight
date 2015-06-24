#include "PID.h"
#include "AlgorithmBasic.h"
#include "string.h"

#include "Algorithm/StatusCalc.h"

#include "Hardware/MPU6050_DMP.h"
#include "Hardware/XRotor.h"

#define LastGyroX() DMP_DATA.GYROx
#define LastGyroY() DMP_DATA.GYROy
#define LastGyroZ() DMP_DATA.GYROz

#include "FlyBasic.h"

PID_Typedef pitch_PID;
PID_Typedef roll_PID;
PID_Typedef yaw_PID;
PID_Typedef alt_PID;

float ExpectedAngle[3] = {0.,0.,0.}; //roll pitch yaw
float ExpectedAltitude = 6.0;

void PID_Init_All(void) {
	PID_Init(&pitch_PID);
	PID_Init(&roll_PID);
	PID_Init(&yaw_PID);
	PID_Init(&alt_PID);
}

void PID_Limiter(int16_t *data, int16_t range) {
	if (*data >  range)  *data =  range;
	if (*data < -range)  *data = -range;
}

void PID_Calc_All() {
	
    PID_Postion_Cal(&roll_PID,	ExpectedAngle[0], status.Roll 	,	LastGyroX());
    PID_Postion_Cal(&pitch_PID, ExpectedAngle[1], status.Pitch	,	LastGyroY());
    PID_Postion_Cal(&yaw_PID,	ExpectedAngle[2], status.Yaw	,	LastGyroZ());
    PID_Postion_Cal(&alt_PID,	ExpectedAltitude, status.Altitude,	0xFFFFFF);
    
    int16_t Pitch = pitch_PID.Output;
    int16_t Roll  = roll_PID.Output;
    int16_t Yaw   = yaw_PID.Output; 
    int16_t Alt   = alt_PID.Output; 
	
	if (status.Altitude < 0) Alt = 0;
	
	PID_Limiter(&Yaw, 10);
	PID_Limiter(&Alt, 10);
	
	Alt += Thro;
	
	if (Flight_Working > 1) {
		Alt /= Flight_Working;
		Flight_Working--;
	}
	
    
	//if((pitch>35)||(pitch<-35)){
//		Motor_Out[1] = 0;    //M1  
//		Motor_Out[2] = 0;    //M2 
//		Motor_Out[3] = 0;    //M3  
//		Motor_Out[0] = 0;    //M4
	//} else {
		//将输出值融合到四个电机 
		/* 
		//X飞行
		Motor_Out[1] = (int16_t)(Thro + Pitch -Roll - Yaw );    //M1  
		Motor_Out[2] = (int16_t)(Thro + Pitch +Roll + Yaw );    //M2 
		Motor_Out[3] = (int16_t)(Thro - Pitch +Roll - Yaw );    //M3  
		Motor_Out[0] = (int16_t)(Thro - Pitch -Roll + Yaw );    //M4
		*/
		//+飞行
		Motor_Out[0] = (int16_t)(Alt - Roll - Yaw );
		Motor_Out[1] = (int16_t)(Alt - Pitch  + Yaw );  
		Motor_Out[2] = (int16_t)(Alt + Roll - Yaw );   // P调 1和3
		Motor_Out[3] = (int16_t)(Alt + Pitch  + Yaw );  //P 调 2和4
	//}
	
	#define Motor_Macro_Limiter(x) if(x>100)x=100;else if(x<0)x=0;
	Motor_Macro_Limiter(Motor_Out[0]);
	Motor_Macro_Limiter(Motor_Out[1]);
	Motor_Macro_Limiter(Motor_Out[2]);
	Motor_Macro_Limiter(Motor_Out[3]);
	
}

void PID_Init(PID_Typedef * PID) {
	memset(PID, 0, sizeof(PID_Typedef));
}

//-----------仅用于角度环和角速度环的位置式PID-----------
void PID_Postion_Cal(PID_Typedef * PID,float target,float measure,int32_t delta)
{

	//-----------位置式PID-----------
	//误差=期望值-测量值
	PID->Error=target-measure;
	
	if (0xFFFFFF != delta) {
		PID->Deriv = delta ;   // 第一层D直接用角速度，效果不错；
	} else {
		PID->Deriv = PID->Error-PID->PreError;
	}	
	PID->Integ = PID->Integ + PID->Error;
  if (PID->Integ>50){
	    PID->Integ = 50; 
	}
	 if (PID->Integ<-50){
	    PID->Integ = -50; 
	 }//PID I项累加，上下限幅正负50
	PID->Output = PID->P * PID->Error + PID->I * PID->Integ + PID->D * PID->Deriv;
	PID->PreError = PID->Error;
}
