#ifndef __COROUTINE_H_
#define __COROUTINE_H_

/******* 硬件相关 *******/
#include "sys.h"
#include "pin_map.h"

/******* 程序相关 *******/
typedef uint8_t u8;
typedef uint16_t u16;

/****** 共用的状态 ******/
typedef enum {
	FWS_IDLE = 0,
	FWS_PREPARE = 1,
	FWS_WARMING = 2,
	FWS_FLYING = 3,
	FWS_LANDING = 4
} Flight_Working_Status;
extern Flight_Working_Status Flight_Working;

/****** 多进程系统 ******/
//using ProtoThreads
#include "PT/pt.h"

#endif
