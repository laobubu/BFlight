//导航程序，负责定时读取传感器 和 计算 PID 和 控制电机

#include "TNavigator.h"
#include "FlyBasic.h"

#include "stdio.h"
#include "math.h"

#include "Hardware/IIC.h"
//#include "Hardware/MPU6050.h"
#include "Hardware/MPU6050_DMP.h"
#include "Hardware/HMC5883L.h"
#include "Hardware/MS5611_I2C.h"
#include "Hardware/XRotor.h"

#include "DataPacker.h"

struct pt ptNavigator;
PT_THREAD(TNavigator(struct pt *pt));

void Init_Navigator(void) {
	PT_INIT(&ptNavigator);
	
	MPU6050_DMP_Initialize();
	//MPU6050_initialize();
	MS5611_Init();
	//HMC58X3_Init();	//这个必须放到MPU6050以后，因为 GY-86 的连接关系比较恶心
	
	PID_Init_All();
}

void Do_Navigator(void) {
	TNavigator(&ptNavigator);
}

////解决 DMP 的数据突然一闪的问题
//struct {
//	float yaw,pitch,roll;
//	uint8_t errorTime;
//} DMP_Last = {0,0,0,0};
//#define DMP_Last_Tolerance 50.0f //两次采样变化的限制值
//#define DMP_Last_Limit 4 //如果错误采样持续这么多次，则视为该采样其实是正确的
//#define DMP_Last_HasError(item) (fabs(DMP_Last.item - DMP_DATA.dmp_##item )>DMP_Last_Tolerance) //判断一个项，比如 yaw,pitch,roll
//#define DMP_Last_Update(item) DMP_Last.item = DMP_DATA.dmp_##item

PT_THREAD(TNavigator(struct pt *pt)) {
	PT_BEGIN(pt);
	
	while(1) {
		PT_TIMER_INTERVAL(pt, 1000/200);  //控制程序频率为 200 Hz
		
		
		//读取气压计
		MS5611_Read();
		//printf("MS5611: Temp %d\t Pres %d \r\n", MS5611.Temperature, MS5611.Pressure);
		
		
		//读取HMC58X3磁力计
		//if (HMC58X3_IsDRDY()) {	//Check if ready 
			//其实 5ms 就可以完成采样，我们的控制频率给他的时间绰绰有余，所以可以不需要判断DRDY
			//HMC58X3_ReadSensor();
			//printf("HMC58X3: %d\t%d\t%d\r\n", HMC58X3.X, HMC58X3.Y, HMC58X3.Z);
			//HMC58X3_ClearDRDY();
		//}
		
		
		//读取MPU6050(无dmp的)
		//if (MPU6050_is_DRY()) {
		//同上
		//MPU6050AccRead(mpu6050);
		//MPU6050GyroRead(mpu6050_2);
		//printf("MPUAcc: %d\t%d\t%d\r\n", mpu6050[0], mpu6050[1], mpu6050[2]);
		//printf("MPUGyr: %d\t%d\t%d\r\n", mpu6050_2[0], mpu6050_2[1], mpu6050_2[2]);
		//MPU6050 clears the flag automatically
		//}
		
		
		//读取MPU6050 DMP
		DMP_Routing();
		//printf("Yaw %f\tPitch %f\tRoll %f\r\n", DMP_DATA.dmp_yaw, DMP_DATA.dmp_pitch, DMP_DATA.dmp_roll);
		//printf("Accl/g: %f\t%f\t%f \n", DMP_DATA.dmp_accx, DMP_DATA.dmp_accy, DMP_DATA.dmp_accz);
		
		
		//读取结束后就交给 PID 处理了
		if (Flight_Working) {
			//检查 DMP 数据是否正常，如果正常则应该是平滑地变化。
			//Maybe the configuration code for DMP has bug....
			//Not a good method to solve but it works...
			//FIXME: !!
			//if ((DMP_Last_HasError(roll) || DMP_Last_HasError(pitch)) && DMP_Last.errorTime < DMP_Last_Limit) {
			//	DMP_Last.errorTime++;
			//} else {
			//	DMP_Last_Update(pitch);
			//	DMP_Last_Update(roll);
			//	DMP_Last_Update(yaw);
			//	DMP_Last.errorTime = 0;
				
				PID_Calc_All(DMP_DATA.dmp_yaw, DMP_DATA.dmp_pitch, DMP_DATA.dmp_roll);
				Motor_SetAllSpeed(Motor_Out[0],Motor_Out[1],Motor_Out[2],Motor_Out[3]);
			//}
		}
		
		//仅供调试用
		DataPacker_Pack(DMP_DATA.dmp_yaw, DMP_DATA.dmp_pitch, DMP_DATA.dmp_roll);
		//DataPacker_Pack(DMP_Last.yaw, DMP_Last.pitch, DMP_Last.roll);
		
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}
