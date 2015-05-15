#include "PID.h"
#include "AlgorithmBasic.h"
#include "string.h"


#include "Hardware/MPU6050_DMP.h"
#include "Hardware/XRotor.h"
#define LastGyroX() DMP_DATA.GYROx
#define LastGyroY() DMP_DATA.GYROy

#include "FlyBasic.h"

PID_Typedef pitch_angle_PID;
PID_Typedef pitch_rate_PID;

PID_Typedef roll_angle_PID;
PID_Typedef roll_rate_PID;

PID_Typedef yaw_angle_PID;
PID_Typedef yaw_rate_PID;

PID_Typedef alt_PID;
PID_Typedef alt_vel_PID;


float ExpectedAngle[3] = {0.,0.,0.}; //roll pitch yaw


void PID_Init_All(void) {
	PID_Init(&pitch_angle_PID);
	//PID_Init(&pitch_rate_PID);
	PID_Init(&roll_angle_PID);
	//PID_Init(&roll_rate_PID);
	PID_Init(&yaw_angle_PID);
	PID_Init(&yaw_rate_PID);
	PID_Init(&alt_PID);
	PID_Init(&alt_vel_PID);
}

void PID_Calc_All(float yaw, float pitch, float roll) {
	/*********************************************************
     PID核心算法部分
    *********************************************************/
  //------------俯仰控制------------
    //参数整定原则为先内后外，故在整定内环时将外环的PID均设为0
    //外环控制。输入为角度,输出为角速度。PID->Output作为内环的输入。
    PID_Postion_Cal(&pitch_angle_PID, ExpectedAngle[1], pitch,LastGyroY());
    
    //内环控制，输入为角速度，输出为PWM增量
    //PID_Postion_Cal(&pitch_rate_PID,pitch_angle_PID.Output, LastGyroY() ,0); 
    //参数整定原则为先内后外，故在整定内环时将外环的PID均设为0
    
    
    //外环控 制。输入为角度,输出为角速度。PID->Output作为内环的输入。
    PID_Postion_Cal(&roll_angle_PID,ExpectedAngle[0], roll, LastGyroX());
    
    //内环控制，输入为角速度，输出为PWM增量
   //PID_Postion_Cal(&roll_rate_PID,roll_angle_PID.Output, LastGyroX() ,0);
    //参数整定原则为先内后外，故在整定内环时将外环的PID均设为0
    

    //外环控 制。输入为角度,输出为角速度。PID->Output作为内环的输入。
    //PID_Postion_Cal(&yaw_angle_PID,ExpectedAngle[2], yaw,0);		//-----Problem
    
    //内环控制，输入为角速度，输出为PWM增量
    //PID_Postion_Cal(&yaw_rate_PID,-2*EXP_ANGLE.Z,DMP_DATA.GYROz,0);		//--------Problem tobe tested
    //参数整定原则为先内后外，故在整定内环时将外环的PID均设为0
    
    
    //基础油门动力
    //Thr = 0.001*RC_DATA.THROTTLE*RC_DATA.THROTTLE;   //RC_DATA.THROTTLE为0到1000,将摇杆油门曲线转换为下凹的抛物线
    //Thro = RC_DATA.THROTTLE;
    //Thro -=  80*DIF_ACC.Z;                             //对Z轴用一次负反馈控制
	//	Thr = Thr / (cosf(Q_ANGLE.Pitch*M_PI_F/180.0f)*cosf(Q_ANGLE.Roll*M_PI_F/180.0f));	//对倾斜做补偿
    
    int16_t Pitch = pitch_angle_PID.Output;
    int16_t Roll  = roll_angle_PID.Output;
    int16_t Yaw   = yaw_angle_PID.Output; 
    
	if((pitch>35)||(pitch<-35)){
		Motor_Out[1] = 0;    //M1  
		Motor_Out[2] = 0;    //M2 
		Motor_Out[3] = 0;    //M3  
		Motor_Out[0] = 0;    //M4
	} else {
		//将输出值融合到四个电机 
		/* 
		//X飞行
		Motor_Out[1] = (int16_t)(Thro + Pitch -Roll - Yaw );    //M1  
		Motor_Out[2] = (int16_t)(Thro + Pitch +Roll + Yaw );    //M2 
		Motor_Out[3] = (int16_t)(Thro - Pitch +Roll - Yaw );    //M3  
		Motor_Out[0] = (int16_t)(Thro - Pitch -Roll + Yaw );    //M4
		*/
		//+飞行
		Motor_Out[0] = (int16_t)(Thro - Roll - Yaw );
		Motor_Out[1] = (int16_t)(Thro - Pitch  + Yaw );  
		Motor_Out[2] = (int16_t)(Thro + Roll - Yaw );   // P调 1和3
		Motor_Out[3] = (int16_t)(Thro + Pitch  + Yaw );  //P 调 2和4
	}
	
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
void PID_Postion_Cal(PID_Typedef * PID,float target,float measure,int32_t dertT)
{
	float termI=1;
	//-----------位置式PID-----------
	//误差=期望值-测量值
	PID->Error=target-measure;
	
	//PID->Deriv= PID->Error-PID->PreError;
	PID->Deriv = dertT ;   // 第一层D直接用角速度，效果不错；
	if ((PID->Error>25)|(PID->Error<-25))
	{
		termI =  0;//角度变大不积分；
 	}
		/*		if(fabs(PID->Output) < Thro )		//比油门还大时不积分
			{
				termI=(PID->													Integ) + (PID->Error) ;
				if(termI > - PID->iLimit && termI < PID->iLimit && PID->Output > - PID->iLimit && PID->Output < PID->iLimit)
						PID->Integ=termI;
			}*/
	PID->Integ = PID->Error+PID->PreError+PID->PrePreError;
  PID->Output=PID->P*PID->Error+PID->I*PID->Integ+PID->D * PID->Deriv;
	PID->PrePreError =  PID->PreError;
  PID->PreError = PID->Error;

	//仅用于角度环和角速度环的
	//if(FLY_ENABLE && offLandFlag)
	//{

	//}
	//else
	//		PID->Integ= 0;
}
