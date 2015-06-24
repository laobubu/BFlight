//负责打包发送数据，也负责处理收到的数据
// ( TMessager 负责的是接收数据的过程，然后交给 DataPacker 处理)

#include "FlyBasic.h"
#include "DataPacker.h"
#include "stdio.h"
#include "string.h"
#include "stm32f1xx_hal.h"

#include "Algorithm/PID.h"

extern PID_Typedef X_PID;
extern PID_Typedef Y_PID;

void DP_HandleParamUpdate(const char name[4], const float value)
{
	if (strcmp(name, "YP")==0) {
		// Handle
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
static struct {
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
	switch (DP_RECV.status) {
		case DP_S_HEAD:
		{
			if (DP_Header[(sizeof(DP_Header) - DP_RECV.byteLeft)] == byte) {
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
		}
		break;
		
		case DP_S_PARAM_NAME:
		{
			if (DP_RECV.paramLeft == 0) {
				DP_RECV_Reset();
				DP_Feed(byte);
			} else {
				DP_RECV.paramName[DP_PARAM_NAME_LEN - (DP_RECV.byteLeft--)] = byte;
				if (DP_RECV.byteLeft == 0) {
					DP_RECV.byteLeft = 4;
					DP_RECV.status = DP_S_PARAM_VALUE;
				}
			}
		}
		break;
		
		case DP_S_PARAM_VALUE:
		{
			DP_RECV.paramValue_char[4 - (DP_RECV.byteLeft--)] = byte;
			if (DP_RECV.byteLeft == 0) {
				DP_HandleParamUpdate(DP_RECV.paramName, DP_RECV.paramValue);
				if (--DP_RECV.paramLeft == 0) {
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

void DP_Init(void)
{
	DP_RECV_Reset();
	memset(&DP_SendPack, 0, sizeof(DP_SendPack));
	DP_SendPack.__header[0] = 0xFE;
	DP_SendPack.__header[1] = 0xFC;
	
	//由于使用的是在 stm32f1xx_it.c 里面拦截HAL的方式，可以不使用 HAL_UART_Receive_IT
}

DP_SendPack_t DP_SendPack;
void DP_Send(void)
{
	HAL_UART_Transmit_DMA(&huart1, (uint8_t*)&DP_SendPack, sizeof(DP_SendPack));
}
