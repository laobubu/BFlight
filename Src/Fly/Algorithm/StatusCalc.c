#include "StatusCalc.h"
#include "AlgorithmBasic.h"

#include "Hardware/MPU6050_DMP.h"
#include "Hardware/HMC5883L.h"
#include "Hardware/MS5611_I2C.h"
#include "Hardware/XRotor.h"
#include "Hardware/Ultrasonic.h"

Status_Typedef status;

void SC_Init_All(void) {
	MPU6050_DMP_Initialize();
	//MPU6050_initialize();
	//MS5611_Init();
	HMC58X3_Init();	//这个必须放到MPU6050以后，因为 GY-86 的连接关系比较恶心
	Ultrasonic_Init();
}

void SC_PreSample(void) {
	//MS5611_Read();	//读气压计
	HMC58X3_ReadSensor();	//读磁力计
	DMP_Routing();	//读mpu6050
}

void SC_PreSample_End(void) {
	
}

//生成新的姿态数据
void SC_Generate(void) {
	status.Pitch = DMP_DATA.dmp_pitch;
	status.Roll = DMP_DATA.dmp_roll;
	status.Yaw = DMP_DATA.dmp_yaw;
	status.Altitude = Ultrasonic.altitude;
}

//做姿态数据数据采样，应定时调用
void SC_Sample(void) {
	Ultrasonic_Trig();		//超声波传感器 采样
	//MS5611_Read();		//读取气压计
	HMC58X3_ReadSensor();	//读取HMC58X3磁力计
	DMP_Routing();			//读取MPU6050 DMP
}
