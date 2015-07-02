#ifndef __COROUTINE_H_
#define __COROUTINE_H_

/******* 硬件相关 *******/
#include "sys.h"
#include "pin_map.h"

/******* 程序相关 *******/

typedef union {
	uint32_t value;
	uint8_t bytes[4];
} uint32andUint8_t;		//便于 I2C 的读取

typedef union {
    uint16_t value;
    uint8_t bytes[2];
} uint16andUint8_t;

typedef uint8_t u8;
typedef uint16_t u16;

/****** 共用的状态 ******/
#include "Algorithm/PID.h"

extern int16_t Motor_Out[4];	//电机输出
extern int16_t Thro;			//油门
extern float ExpectedAngle[3]; 	//期望角度
extern float ExpectedAltitude;  //期望高度

extern char Flight_Working;

/****** 多进程系统 ******/
//using ProtoThreads
#include "PT/pt.h"

/******* 实用函数 *******/
#define delayThread(pt,x)	PT_TIMER_DELAY(pt,x)	//用于进程函数的长时间延迟（x毫秒）

#endif
