#ifndef __status_calc_h_
#define __status_calc_h_

// 姿态计算程序

typedef struct {
	float Yaw,Pitch,Roll;
	float Altitude;
} Status_Typedef;

extern Status_Typedef status;

void SC_Init_All(void);		//初始化所有有关姿态计算的东西（含有外设）

void SC_PreSample(void);		//初始化（开机后10s）时候用的采样
void SC_PreSample_End(void);	//预采样结束后的调用函数

void SC_Generate(void);		//生成新的姿态数据
void SC_Sample(void); 		//做姿态数据数据采样，应定时调用

#endif
