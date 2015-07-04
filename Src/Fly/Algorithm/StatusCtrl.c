#include "sys.h"
#include "AlgorithmBasic.h"
#include "StatusCtrl.h"

#include "string.h"

#include "Hardware/MPU6050_DMP.h"
#include "Hardware/XRotor.h"

StatusCtrl_Typedef status_ctrl;

void SCx_Init(void)
{
	memset(&status_ctrl, 0, sizeof(status_ctrl));
	PID_Init(&status_ctrl.PID_pitch, 	PID_MODE_DERIVATIV_SET, 	0.005f);
	PID_Init(&status_ctrl.PID_roll, 	PID_MODE_DERIVATIV_SET, 	0.005f);
	PID_Init(&status_ctrl.PID_yaw, 		PID_MODE_DERIVATIV_SET, 	0.005f);
	PID_Init(&status_ctrl.PID_alt, 		PID_MODE_DERIVATIV_CALC, 	0.005f);
}

static volatile uint32_t lastPIDTime = 0;
void SCx_Process(void)
{
	float dt;
	
	if (lastPIDTime == 0) {
		lastPIDTime = millis();
		return;
	}
	
	dt = (millis() - lastPIDTime)/1000.0f;
	lastPIDTime = millis();
	
	status_ctrl.Pitch = PID_Postion_Cal(&status_ctrl.PID_pitch, status_ctrl.expectedStatus.Pitch, 		status.Pitch 	,	DMP_DATA.GYROy	, dt);
	status_ctrl.Roll  = PID_Postion_Cal(&status_ctrl.PID_roll,  status_ctrl.expectedStatus.Roll, 		status.Roll		,	DMP_DATA.GYROx	, dt);
	status_ctrl.Yaw   = PID_Postion_Cal(&status_ctrl.PID_yaw,	status_ctrl.expectedStatus.Yaw, 		status.Yaw		,	DMP_DATA.GYROz	, dt);
	status_ctrl.Alt   = PID_Postion_Cal(&status_ctrl.PID_alt,	status_ctrl.expectedStatus.Altitude , 	status.Altitude	,	0				, dt);
	
	//If the altitude value has problem...
	if (status.Altitude < 3) status_ctrl.Alt = 0;
	
//	//X copter
//	status_ctrl.Motor_Out[1] = (int16_t)(status_ctrl.Thro + status_ctrl.Pitch - status_ctrl.Roll - status_ctrl.Yaw );  
//	status_ctrl.Motor_Out[2] = (int16_t)(status_ctrl.Thro + status_ctrl.Pitch + status_ctrl.Roll + status_ctrl.Yaw );  
//	status_ctrl.Motor_Out[3] = (int16_t)(status_ctrl.Thro - status_ctrl.Pitch + status_ctrl.Roll - status_ctrl.Yaw );  
//	status_ctrl.Motor_Out[0] = (int16_t)(status_ctrl.Thro - status_ctrl.Pitch - status_ctrl.Roll + status_ctrl.Yaw );  

	
	//+ copter
	status_ctrl.Motor_Out[0] = (int16_t)(status_ctrl.Thro + status_ctrl.Alt - status_ctrl.Roll   - status_ctrl.Yaw );
	status_ctrl.Motor_Out[1] = (int16_t)(status_ctrl.Thro + status_ctrl.Alt - status_ctrl.Pitch  + status_ctrl.Yaw );  
	status_ctrl.Motor_Out[2] = (int16_t)(status_ctrl.Thro + status_ctrl.Alt + status_ctrl.Roll   - status_ctrl.Yaw );  
	status_ctrl.Motor_Out[3] = (int16_t)(status_ctrl.Thro + status_ctrl.Alt + status_ctrl.Pitch  + status_ctrl.Yaw );  
	
	#define Motor_Macro_Limiter(x) if(x>100)x=100;else if(x<0)x=0;
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
