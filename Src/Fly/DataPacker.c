//负责打包发送数据，也负责处理收到的数据
// ( TMessager 负责的是接收数据的过程，然后交给 DataPacker 处理)

#include "FlyBasic.h"
#include "DataPacker.h"
#include "stdio.h"
#include "string.h"
#include "stm32f1xx_hal.h"

#include "Hardware/XRotor.h"
#include "Hardware/Ultrasonic.h"

#include "Algorithm/PID.h"

//处理 32byte 的收到的包
void DataPacker_ProcessRecvPack(unsigned char *pack) {
	static union {
		uint16_t u16;
		int integer;
		struct {
			float *number;
			PID_Typedef *pid;
		} pid;
		char *pChar;
	} tmp;
	
	if (pack[1] == 0xBB) {					//Crazepony控制指令
		Thro = tmp.u16 = pack[3]<<8 | pack[2];
		if (pack[31] == 0x00) tmp.u16 = 0;			//关断
		Motor_SetAllSpeed(tmp.u16,tmp.u16,tmp.u16,tmp.u16);
		return;
	}
	
	if (pack[1] == 0xCC) {
		strsolve:
		tmp.pid.pid = 0;
			 if (pack[2] == 'y')	tmp.pid.pid = &yaw_PID;
		else if (pack[2] == 'r')	tmp.pid.pid = &roll_PID;
		else if (pack[2] == 'p')	tmp.pid.pid = &pitch_PID;
		else if (pack[2] == 'a')	tmp.pid.pid = &alt_PID;
		
		if (tmp.pid.pid) {	//this is a PID control CMD
				 if (pack[3] == 'p')	tmp.pid.number = &tmp.pid.pid->P;
			else if (pack[3] == 'i')	tmp.pid.number = &tmp.pid.pid->I;
			else if (pack[3] == 'd')	tmp.pid.number = &tmp.pid.pid->D;
			else if (pack[3] == 'e')	tmp.pid.number = &tmp.pid.pid->Desired;
			
			sscanf((const char*)(pack+4),"%f", tmp.pid.number);
			
			if (pack[3] == 'e') {
					 if (pack[2] == 'r') ExpectedAngle[0] = *tmp.pid.number;
				else if (pack[2] == 'p') ExpectedAngle[1] = *tmp.pid.number;
				else if (pack[2] == 'y') ExpectedAngle[2] = *tmp.pid.number;
				else if (pack[2] == 'a') ExpectedAltitude = *tmp.pid.number;
			}
		} else {
			if (pack[2] == 's') {		//停止并重置
				Flight_Working = 0;
				PID_Init_All();
				Motor_SetAllSpeed(0,0,0,0);
			} else 
			if (pack[2] == 'g') {
				Flight_Working = 1;
			} else 
			if (pack[2] == 't') {
				sscanf((const char*)(pack+3),"%d", &tmp.integer);
				Motor_SetAllSpeed(tmp.integer,tmp.integer,tmp.integer,tmp.integer);
				Thro = tmp.integer;
			}
		}
		
		tmp.pChar = strchr((const char*)(pack+2), ';');
		if (tmp.pChar) {
			while(tmp.pChar[1] == ' ') tmp.pChar++;
			pack = (unsigned char*)tmp.pChar-1;
			goto strsolve;
		}
		
		return;
	}
}



static uint8_t PackData[20] = {0x88, 0xAF, 0x1C};
extern int16_t Motor_Out[4];
extern UART_HandleTypeDef huart1;
void DataPacker_Pack(float yaw, float pitch, float roll) {
	uint8_t j, *pd = PackData+3;
	int16_t tmp;
	
	if (huart1.State == HAL_UART_STATE_BUSY_TX || huart1.State == HAL_UART_STATE_BUSY_TX_RX)
		return;
	
	#define PUSH_INT16(x) tmp = x;  *pd++ = tmp >> 8;	*pd++ = tmp & 0xFF;
	
	PUSH_INT16(pitch + 500);
	PUSH_INT16(roll + 500);
	PUSH_INT16(yaw + 500);
	
	PUSH_INT16(Motor_Out[0]);
	PUSH_INT16(Motor_Out[1]);
	PUSH_INT16(Motor_Out[2]);
	PUSH_INT16(Motor_Out[3]);
	
	PUSH_INT16(Ultrasonic.altitude);
	
	PackData[sizeof(PackData)-1] = 0;
	for ( j = 3; j < sizeof(PackData)-2; j++)
		PackData[sizeof(PackData)-1] += PackData[j];
	
	HAL_UART_Transmit_IT(&huart1, PackData, sizeof(PackData));
}
