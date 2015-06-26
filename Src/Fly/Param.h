#ifndef __PARAM__
#define __PARAM__

#include <stdint.h>

//定义各种参数，名字保持在4个字之内最佳
#define PARAM_LIST(DEFINE_PARAM)	\
	DEFINE_PARAM(float,   RFix)		/* Roll  零偏 */			\
	DEFINE_PARAM(float,   PFix)		/* Pitch 零偏 */			\
	DEFINE_PARAM(float,   BOff)		/* 避障时的角度变化量 */	\

	
//定义结构体
#define PARAM_HELPER_FUN1(type, name)	type name;
typedef struct {PARAM_LIST(PARAM_HELPER_FUN1)} Param_t;

//全局参数配置
extern Param_t Param;

//各种函数
void Param_Reset(void);		//初始化默认值

#endif
