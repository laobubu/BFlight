#include "StatusCalc.h"
#include "AlgorithmBasic.h"

#include "DataPacker.h"
#include "string.h"

#include "Hardware/MPU6050_DMP.h"
#include "Hardware/HMC5883L.h"
#include "Hardware/MS5611_I2C.h"
#include "Hardware/XRotor.h"
#include "Hardware/Ultrasonic.h"

Status_Typedef status;
float av;
float preav = 0 ;
float prealt ;
typedef struct {
	float SUM;
	uint16_t SAMPLE;
} AVGCALC;
#define AVGCALC_PUSH(obj,val)  obj.SUM+=val;obj.SAMPLE++;
#define AVGCALC_GET(obj) (obj.SUM/obj.SAMPLE)

static struct {
	AVGCALC Sonar, Baro, Mag;
	uint16_t BAROCNT;
} Presample;

static float Yaw_MagMinusDMP = 180.0f;
static float FloorAltitudeSonar = 0.0;
static float FloorAltitudeBaro = 0.0;

//static char SensorPending_DMP = 0;
static char SensorPending_HMC58X3 = 0;

void SC_Init_All(void) {
	MPU6050_DMP_Initialize();
	//MPU6050_initialize();
	//MS5611_Init();
	//HMC58X3_Init();	//这个必须放到MPU6050以后，因为 GY-86 的连接关系比较恶心
	Ultrasonic_Init();
	
	memset(&Presample, 0, sizeof(Presample));
}

void SC_PreSample(void) {
	SC_Sample();	
	
//	HMC58X3_ReadSensor();	//读磁力计
//	DMP_Routing();			//读mpu6050
	
//	if (Presample.BAROCNT != 10) {
//		Presample.BAROCNT++;
//	} else  {
//		AVGCALC_PUSH(Presample.Baro, MS5611.Altitude);
//	}
	AVGCALC_PUSH(Presample.Sonar,Ultrasonic.altitude);
	prealt = Ultrasonic.altitude;
	
//	if (SensorPending_HMC58X3) {
//		HMC58X3_ReadSensor();
//		SensorPending_HMC58X3 = 0;
//		AVGCALC_PUSH(Presample.Mag,  HMC58X3.Yaw);
//	}
}

void SC_PreSample_End(void) {
	SC_PreSample();
	
//	Yaw_MagMinusDMP = angleNorm(AVGCALC_GET(Presample.Mag) - DMP_DATA.dmp_yaw) + 180.0f;
//	FloorAltitudeBaro = AVGCALC_GET(Presample.Baro);
	FloorAltitudeSonar = AVGCALC_GET(Presample.Sonar);
	
	SC_ResetYaw();
}

void SC_ResetYaw(void) {
	Yaw_MagMinusDMP = -DMP_DATA.dmp_yaw;
}

//生成新的姿态数据
void SC_Generate(void) {
  //	float temp1, temp2;
	
	status.Pitch = DMP_DATA.dmp_pitch;
	status.Roll = DMP_DATA.dmp_roll;
	status.Yaw = angleNorm2(DMP_DATA.dmp_yaw + Yaw_MagMinusDMP);
	//status.Altitude = Ultrasonic.altitude;  // 旧的高度由超声波读取
	av = Ultrasonic.altitude - prealt;
	if ((av - preav > 5)||(av - preav<-5)){
	    status.Altitude = 0.01*Ultrasonic.altitude + 0.99 * prealt;
	}
	else {
	   status.Altitude =  0.1*Ultrasonic.altitude + 0.9 * prealt;
		 preav =  av; 
	}
	
	prealt = status.Altitude; //奇怪的滤波程序；
	
}

//做姿态数据数据采样，应定时调用
void SC_Sample(void) {
	Ultrasonic_Trig();		//超声波传感器 采样
//	MS5611_Read();		//读取气压计
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
//	SensorPending_HMC58X3 = 1;
}
