#ifndef _AlgorithmBasic_H_1
#define _AlgorithmBasic_H_1

#include "math.h"
#include "stdint.h"

#define PI  3.1415926f

// Fast inverse square-root 快速计算 1/Sqrt(x)
float InvSqrt(float x);

// Safe Functions
float asin_s(float v);

//将角度限制为 0°-360°
float angleNorm(float angle);

//将角度限制为 -180°-180°
float angleNorm2(float angle);

#endif
