#ifndef __MS5611_I2C_H_
#define __MS5611_I2C_H_

#include "stdint.h"

#define MS5611Address 0xEE	//I2C地址（假定CSB引脚被拉低，则1位就是1）

//数据格式
typedef struct {
	int32_t Temperature;		//温度，如 2007 就是 20.07℃
	float Pressure;			    //气压，100009 就是 1000.09 mbar
	float Altitude;				//这个……等以后再看吧
} MS5611_TypeDef;

//传感器对象
extern MS5611_TypeDef MS5611;

//API
void MS5611_Init(void);		//初始化
void MS5611_Read(void);		//更新数据

#endif
