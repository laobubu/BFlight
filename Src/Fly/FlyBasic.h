#ifndef __COROUTINE_H_
#define __COROUTINE_H_

/******* 硬件相关 *******/
#include "sys.h"
#include "pin_map.h"

/******* 程序相关 *******/
typedef uint8_t u8;
typedef uint16_t u16;

/****** 共用的状态 ******/
extern char Flight_Working;

/****** 多进程系统 ******/
//using ProtoThreads
#include "PT/pt.h"

#endif
