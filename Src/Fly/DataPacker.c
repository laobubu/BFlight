//负责打包发送数据，也负责处理收到的数据
// ( TMessager 的工作是：1. 定时发送最新姿态数据 2. 检查最近一次通讯时间并做出失联时的反应 )

#include "FlyBasic.h"
#include "DataPacker.h"
#include "stdio.h"
#include "string.h"

//Things that params affect.
#include "Algorithm/PID.h"
#include "Hardware/XRotor.h"
#include "Hardware/PX4Flow.h"

extern PID_Typedef X_PID;
extern PID_Typedef Y_PID;
extern float ExpectedPos[2]; //X,Y

#define DP_IS_PARAM_NAME(cmp)	(memcmp(name, cmp "\0\0\0", 4)==0)

void DP_HandleParamUpdate(char name[4], float value)
{
	static union {
		struct {
			float *number;
			PID_Typedef *pid;
		} pid;
	} tmp;
	
	if (name[0] == 'g') {	//param whose name begins with 'g' shall be PID
		
		//find the PID Object
			 if (name[1] == 'y')	tmp.pid.pid = &yaw_PID;
		else if (name[1] == 'r')	tmp.pid.pid = &roll_PID;
		else if (name[1] == 'p')	tmp.pid.pid = &pitch_PID;
		else if (name[1] == 'a')	tmp.pid.pid = &alt_PID;
		else if (name[1] == 'X')	tmp.pid.pid = &X_PID;
		else if (name[1] == 'Y')	tmp.pid.pid = &Y_PID;
		else						return;
		
		//find the PID Gain factor
			 if (name[2] == 'p')	tmp.pid.number = &tmp.pid.pid->P;
		else if (name[2] == 'i')	tmp.pid.number = &tmp.pid.pid->I;
		else if (name[2] == 'd')	tmp.pid.number = &tmp.pid.pid->D;
		else if (name[2] == 'e')	tmp.pid.number = &tmp.pid.pid->Desired;
		
		*tmp.pid.number = value;
		
		if (name[2] == 'e') {		//something special
				 if (name[1] == 'r') ExpectedAngle[0] = *tmp.pid.number;
			else if (name[1] == 'p') ExpectedAngle[1] = *tmp.pid.number;
			else if (name[1] == 'y') ExpectedAngle[2] = *tmp.pid.number;
			else if (name[1] == 'a') ExpectedAltitude = *tmp.pid.number;
			else if (name[1] == 'X') ExpectedPos[0] = *tmp.pid.number;
			else if (name[1] == 'Y') ExpectedPos[1] = *tmp.pid.number;
		}
		
		return;
	}
	
	if (DP_IS_PARAM_NAME("Thro")) {	//油门
		Thro = value;
		Motor_SetAllSpeed(Thro, Thro, Thro, Thro);
		return;
	}
	
	if (DP_IS_PARAM_NAME("Work")) {	//总开关，为 0 表示不开
		Flight_Working = value;
		if (!Flight_Working) {
			PID_Init_All();
			Motor_SetAllSpeed(0,0,0,0);
		} else {
			//reset sensor
			PX4Flow_Reset();
		}
		return;
	}
	
	if (DP_IS_PARAM_NAME("YP")) {
		
		return;
	}
}



////////////////////////////////////////////////////////////////////
// 下面的代码是 protocol implementation
// DO NOT CARE....

extern UART_HandleTypeDef huart1;

typedef enum {
	DP_S_HEAD = 0,
	DP_S_COUNT,
	DP_S_PARAM_NAME,
	DP_S_PARAM_VALUE
} DP_Status_t;

#define DP_PARAM_NAME_LEN 4

#pragma anon_unions
static volatile struct {
	uint8_t byteLeft;
	uint8_t paramLeft;
	DP_Status_t status;
	char paramName[DP_PARAM_NAME_LEN];
	union {
		char paramValue_char[4];
		float paramValue;
	};
} DP_RECV;

uint64_t DP_LastUpdate = 0;
const char DP_Header[] = {0xFE, 0xFC};

void DP_Feed(char byte)	//shall be called by interrupt function
{
	uint16_t temp1;
	switch (DP_RECV.status) {
		case DP_S_HEAD:
		{
			temp1 = (sizeof(DP_Header) - DP_RECV.byteLeft);
			if (DP_Header[temp1] == byte) {
				DP_RECV.byteLeft--;
				if (DP_RECV.byteLeft == 0) 
					DP_RECV.status = DP_S_COUNT;
			} else {
				DP_RECV_Reset();
			}
		}
		break;
		
		case DP_S_COUNT:
		{
			DP_LastUpdate = millis();
			DP_RECV.paramLeft = byte;
			DP_RECV.byteLeft = DP_PARAM_NAME_LEN;
			DP_RECV.status = DP_S_PARAM_NAME;
			if (DP_RECV.paramLeft == 0) {	//heartbeat
				DP_RECV_Reset();
			}
		}
		break;
		
		case DP_S_PARAM_NAME:
		{
			temp1 = DP_PARAM_NAME_LEN - DP_RECV.byteLeft;
			DP_RECV.byteLeft--;
			DP_RECV.paramName[temp1] = byte;
			if (DP_RECV.byteLeft == 0) {
				DP_RECV.byteLeft = 4;
				DP_RECV.status = DP_S_PARAM_VALUE;
			}
		}
		break;
		
		case DP_S_PARAM_VALUE:
		{
			DP_RECV.paramValue_char[4 - (DP_RECV.byteLeft--)] = byte;
			if (DP_RECV.byteLeft == 0) {
				DP_HandleParamUpdate(DP_RECV.paramName, DP_RECV.paramValue);
				DP_RECV.paramLeft--;
				if (DP_RECV.paramLeft == 0) {
					DP_RECV_Reset();
				} else {
					DP_RECV.byteLeft = DP_PARAM_NAME_LEN;
					DP_RECV.status = DP_S_PARAM_NAME;
				}
			}
		}
		break;
	}
}

void DP_RECV_Reset(void)
{
	DP_RECV.paramLeft = 0;
	DP_RECV.byteLeft = sizeof(DP_Header);
	DP_RECV.status = DP_S_HEAD;
}

static uint8_t byte;
void DP_Init(void)
{
	DP_RECV_Reset();
	memset(&DP_SendPack, 0, sizeof(DP_SendPack));
	DP_SendPack.__header[0] = 0xFE;
	DP_SendPack.__header[1] = 0xFC;
	
	//由于使用的是在 stm32f1xx_it.c 里面拦截HAL的方式，可以不使用 HAL_UART_Receive_IT
	
	HAL_UART_Receive_IT(&huart1, &byte, 1);
	USART1->CR1 |= USART_CR1_RXNEIE;
}

DP_SendPack_t DP_SendPack;
void DP_Send(void)
{
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)&DP_SendPack, sizeof(DP_SendPack));
}
