#ifndef __PARAM__
#define __PARAM__

#include <stdint.h>

//定义各种参数，名字保持在4个字之内最佳
#define PARAM_LIST(DEFINE_PARAM)	\
	DEFINE_PARAM(float,   RFix)		/* Roll  零偏 */			\
	DEFINE_PARAM(float,   YFix)		/* 在直角转弯时候的 Pitch 和 Roll 补偿 */		\
	DEFINE_PARAM(float,   PFix)		/* Pitch 零偏 */			\
	DEFINE_PARAM(float,   BOff)		/* 避障时的角度变化量 */	\
	DEFINE_PARAM(uint8_t, Mode)		/* 模式选择，具体看TNavigator */	\
	DEFINE_PARAM(float,   ThUp)		/* 油门线性提高速度（用于起飞，时间单位5ms） */	\
	DEFINE_PARAM(float,   ThDn)		/* 油门线性降低速度（用于降落，时间单位5ms） */	\
	DEFINE_PARAM(float,   FAA1)		/* Plan System Landing Pitch Fallback */	\
	DEFINE_PARAM(float,   PiGo)		/* 前进时候的 Pitch */			\
  DEFINE_PARAM(float,   PiG1)		/* 前进时候的 Pitch */			\
	DEFINE_PARAM(float,   PiG2)		/* 前进时候的 Pitch (用于Mode2短线) */			\
	DEFINE_PARAM(float,   PiG3)		/* 前进时候的 Pitch */			\
	DEFINE_PARAM(float,   PiBk)		/* 后退时候的 Pitch */			\
	DEFINE_PARAM(float,   SGGD)		/* 甩锅高度 */			\
	DEFINE_PARAM(float,   YaDn)		/* Yaw 匀速变化速度 */			\
	DEFINE_PARAM(float,   YaGh)		/* Yaw Ghost */		\
	DEFINE_PARAM(float,   YFi1)			\
  DEFINE_PARAM(float,   YFi2)			\
  DEFINE_PARAM(float,   YFi3)			\
	DEFINE_PARAM(float,   YFi4)			\
	DEFINE_PARAM(uint32_t,Tmot)		/* 最长飞行时间 */	\
		

	
//定义结构体
#define PARAM_HELPER_FUN1(type, name)	type name;
typedef struct {PARAM_LIST(PARAM_HELPER_FUN1)} Param_t;

//全局参数配置
extern Param_t Param;

//各种函数
void Param_Reset(void);		//初始化默认值

#endif
