#include "StatusCalc.h"
#include "AlgorithmBasic.h"

#include "Hardware/MPU6050_DMP.h"
#include "Hardware/HMC5883L.h"
#include "Hardware/MS5611_I2C.h"
#include "Hardware/XRotor.h"
#include "Hardware/Ultrasonic.h"

Status_Typedef status;

static float Yaw_MagMinusDMP = 180.0f;

//static char SensorPending_DMP = 0;
static char SensorPending_HMC58X3 = 0;

void SC_Init_All(void) {
	MPU6050_DMP_Initialize();
	//MPU6050_initialize();
	MS5611_Init();
	HMC58X3_Init();	//这个必须放到MPU6050以后，因为 GY-86 的连接关系比较恶心
	Ultrasonic_Init();
}

void SC_PreSample(void) {
	SC_Sample();	
	
//	HMC58X3_ReadSensor();	//读磁力计
//	DMP_Routing();			//读mpu6050
	if (SensorPending_HMC58X3) {
		HMC58X3_ReadSensor();
		Yaw_MagMinusDMP = angleNorm(HMC58X3.Yaw - DMP_DATA.dmp_yaw) + 180.0f;
		SensorPending_HMC58X3 = 0;
	}
}

void SC_PreSample_End(void) {
	SC_PreSample();
	//MS5611_SetFloor();
	//MS5611.floorAltitude = MS5611.Altitude - Ultrasonic.altitude;
}

//生成新的姿态数据
void SC_Generate(void) {
	status.Pitch = DMP_DATA.dmp_pitch;
	status.Roll = DMP_DATA.dmp_roll;
	status.Yaw = angleNorm(DMP_DATA.dmp_yaw + Yaw_MagMinusDMP) - 180.0f;
	status.Altitude = Ultrasonic.altitude;
}

//做姿态数据数据采样，应定时调用
void SC_Sample(void) {
	Ultrasonic_Trig();		//超声波传感器 采样
	MS5611_Read();		//读取气压计
	//if (SensorPending_DMP) {
		DMP_Routing();
	//	SensorPending_DMP = 0;
	//}
}

//StatusCalc Callback for sensor
void SC_Callback_DMP(void) {
	//SensorPending_DMP = 1;
}

void SC_Callback_HMC58X3(void) {
	SensorPending_HMC58X3 = 1;
}
