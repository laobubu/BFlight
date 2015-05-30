#include "AlgorithmBasic.h"

// Fast inverse square-root 快速计算 1/Sqrt(x)
float InvSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = 0x5f3759df - ((*(long*)&y)>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

// a varient of asin() that checks the input ranges and ensures a
// valid angle as output. If nan is given as input then zero is
// returned.
float asin_s(float v)
{
	if (v >= 1.0) {
		return PI/2;
	}
	if (v <= -1.0) {
		return -PI/2;
	}
	return asin(v);
}


float angleNorm(float angle) { //将角度限制为 0°-360°
	while (angle<0) 	angle+=360.0;
	while (angle>360.0)	angle-=360.0;
	return angle;
}
