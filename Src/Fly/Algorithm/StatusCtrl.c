#include "sys.h"
#include "AlgorithmBasic.h"
#include "StatusCtrl.h"

#include "string.h"

#include "Hardware/MPU6050_DMP.h"
#include "Hardware/XRotor.h"

StatusCtrl_Typedef status_ctrl;
//int16_t ThroE = 25;   //模拟摇杆
//int counter = 0 ;


void SCx_Init(void)
{
	memset(&status_ctrl, 0, sizeof(status_ctrl));
	PID_Init(&status_ctrl.PID_pitch, 	PID_MODE_DERIVATIV_SET, 	0.005f);
	PID_Init(&status_ctrl.PID_roll, 	PID_MODE_DERIVATIV_SET, 	0.005f);
	PID_Init(&status_ctrl.PID_yaw, 		PID_MODE_DERIVATIV_SET, 	0.005f);
	PID_Init(&status_ctrl.PID_alt, 		PID_MODE_DERIVATIV_CALC, 	0.005f);
}

void SCx_BeforeFly(void)
{
	status_ctrl.ghostExpect.Pitch = 0;
	status_ctrl.ghostExpect.Roll  = 0;
	status_ctrl.ghostExpect.Yaw   = 0;
	status_ctrl.ghostExpect.Altitude = 0;
	
	pid_Reset_Integral(&status_ctrl.PID_pitch);
	pid_Reset_Integral(&status_ctrl.PID_roll);
	pid_Reset_Integral(&status_ctrl.PID_yaw);
	pid_Reset_Integral(&status_ctrl.PID_alt);
}

void SCx_ProcessOutput(void)
{
//	//X copter
//	status_ctrl.Motor_Out[1] = (int16_t)(status_ctrl.Thro + status_ctrl.Pitch - status_ctrl.Roll - status_ctrl.Yaw );  
//	status_ctrl.Motor_Out[2] = (int16_t)(status_ctrl.Thro + status_ctrl.Pitch + status_ctrl.Roll + status_ctrl.Yaw );  
//	status_ctrl.Motor_Out[3] = (int16_t)(status_ctrl.Thro - status_ctrl.Pitch + status_ctrl.Roll - status_ctrl.Yaw );  
//	status_ctrl.Motor_Out[0] = (int16_t)(status_ctrl.Thro - status_ctrl.Pitch - status_ctrl.Roll + status_ctrl.Yaw );  

	
	//+ copter
//	if(status_ctrl.Thro > ThroE ){
//	       counter++;
//	}
//	if(status_ctrl.Thro < ThroE ){
//	      ThroE = status_ctrl.Thro ; 
//	}
//	if (counter == 10){
//					if(status_ctrl.Thro > ThroE ){
//							 ThroE += 1 ;
//					}
//					counter = 0;
//   }
	status_ctrl.Motor_Out[0] = (int16_t)( 10.0f * (status_ctrl.Thro + status_ctrl.Alt - status_ctrl.Roll   - status_ctrl.Yaw ));
	status_ctrl.Motor_Out[1] = (int16_t)( 10.0f * (status_ctrl.Thro + status_ctrl.Alt - status_ctrl.Pitch  + status_ctrl.Yaw ));  
	status_ctrl.Motor_Out[2] = (int16_t)( 10.0f * (status_ctrl.Thro + status_ctrl.Alt + status_ctrl.Roll   - status_ctrl.Yaw ));  
	status_ctrl.Motor_Out[3] = (int16_t)( 10.0f * (status_ctrl.Thro + status_ctrl.Alt + status_ctrl.Pitch  + status_ctrl.Yaw ));  
	
	#define Motor_Macro_Limiter(x) if(x>1000)x=1000;else if(x<0)x=0;
	Motor_Macro_Limiter(status_ctrl.Motor_Out[0]);
	Motor_Macro_Limiter(status_ctrl.Motor_Out[1]);
	Motor_Macro_Limiter(status_ctrl.Motor_Out[2]);
	Motor_Macro_Limiter(status_ctrl.Motor_Out[3]);
			
	Motor_SetAllSpeed(
		status_ctrl.Motor_Out[3],
		status_ctrl.Motor_Out[1],
		status_ctrl.Motor_Out[2],
		status_ctrl.Motor_Out[0]
	);
}

void SCx_ProcessAlt(void)
{
	static volatile uint32_t lastPIDTime = 0;
	float dt;
	
	if (lastPIDTime == 0) {
		lastPIDTime = millis();
		return;
	}
	
	dt = (millis() - lastPIDTime)/1000.0f;
	lastPIDTime = millis();
	
	SC_Generate();
	
	status_ctrl.Alt   = PID_Postion_Cal(&status_ctrl.PID_alt,	
													fminf(
															(status_ctrl.expectedStatus.Altitude + status_ctrl.ghostExpect.Altitude), 
															status.Altitude + 8 + status_ctrl.ghostExpect.Altitude
													)
													, 	status.Altitude	,	0	, dt);
	
	//If the altitude value has problem...
	if (status.Altitude < 3) status_ctrl.Alt = 0;
}

void SCx_ProcessAngle(void)
{
	static volatile uint32_t lastPIDTime = 0;
	float dt;
	
	if (lastPIDTime == 0) {
		lastPIDTime = millis();
		return;
	}
	
	dt = (millis() - lastPIDTime)/1000.0f;
	lastPIDTime = millis();
	
	SC_Generate();
	
	status_ctrl.Pitch = PID_Postion_Cal(&status_ctrl.PID_pitch, angleNorm2(status_ctrl.expectedStatus.Pitch + status_ctrl.ghostExpect.Pitch -	status.Pitch) 	, 0 ,	DMP_DATA.GYROy	, dt);
	status_ctrl.Roll  = PID_Postion_Cal(&status_ctrl.PID_roll,  angleNorm2(status_ctrl.expectedStatus.Roll  + status_ctrl.ghostExpect.Roll  -	status.Roll	)		, 0 ,	DMP_DATA.GYROx	, dt);
	status_ctrl.Yaw   = PID_Postion_Cal(&status_ctrl.PID_yaw,	  angleNorm2(status_ctrl.expectedStatus.Yaw   + status_ctrl.ghostExpect.Yaw   -	status.Yaw)			, 0 ,	DMP_DATA.GYROz	, dt);
}
