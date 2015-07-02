#ifndef __FILTERS_H_
#define	__FILTERS_H_

#include <math.h>


///////////////////////////////////
///Alpha Filter
typedef struct {
	float alpha;
	float last;
} AlphaFilter;
AlphaFilter AF_Create(float freq_hz);
float AF_Update(AlphaFilter *filter, float value);


///////////////////////////////////
///Kalman Filter (KF)

typedef struct {
	float q; //process noise covariance
	float r; //measurement noise covariance
	float k; //kalman gain
	float p; //estimation error cvariance
	float x; //value
} KalmanFilter;
KalmanFilter KF_Create(float process_noise, float measure_noise, float default_value);
float KF_Update(KalmanFilter *filter, float value);

#endif
