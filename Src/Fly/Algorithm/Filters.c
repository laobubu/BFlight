#include "Filters.h"

///////////////////////////////////
///Alpha Filter
AlphaFilter AF_Create(float freq_hz) 
{
	float rc = 1/((3.14159265f*2) * freq_hz);
	AlphaFilter rtn;
	rtn.last = 0;
	rtn.alpha =  1 / (1 + rc);

	return rtn;
}
float AF_Update(AlphaFilter *filter, float value)
{
	filter->last += filter->alpha * (value - filter->last);
	return filter->last;
}

///////////////////////////////////
///Kalman Filter (KF)
KalmanFilter KF_Create(float process_noise, float measure_noise, float default_value)
{
	KalmanFilter rtn;
	rtn.q = process_noise;
	rtn.r = measure_noise;
	//rtn.p = sqrt((process_noise * process_noise) + (measure_noise * measure_noise));
	rtn.p = 1;
	rtn.x = default_value;

	return rtn;
}
float KF_Update(KalmanFilter *filter, float value)
{
	//prediction update
	filter->p += filter->q;
	
	//measurement update
	filter->k = filter->p / (filter->p + filter->r);
	filter->x += filter->k * (value - filter->x);
	filter->p *= 1 - filter->k;

	return filter->x;
}
