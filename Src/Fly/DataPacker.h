#ifndef __DATAPACKER__
#define __DATAPACKER__

//负责打包发送数据，也负责处理收到的数据

#include <stdint.h>

typedef __packed struct {
	char __header[2];
	float Pitch, Roll, Yaw, Alt;
	uint8_t Motor[4];
} DP_SendPack_t;

extern uint64_t DP_LastUpdate;
extern DP_SendPack_t DP_SendPack;

void DP_RECV_Reset(void);
void DP_Feed(char byte);	//shall be called by interrupt function
void DP_Init(void);
void DP_Send(void);

#endif
