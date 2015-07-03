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
	PID_Init(&status_ctrl.PID_pitch);
	PID_Init(&status_ctrl.PID_roll);
	PID_Init(&status_ctrl.PID_yaw);
	PID_Init(&status_ctrl.PID_alt);
	status_ctrl.PID_OutputLimit = 10.0f;
}

void SCx_Process(void)
{
	int16_t Pitch ;
	int16_t Roll  ;
	int16_t Yaw   ;
	int16_t Alt   ;
	
	PID_Postion_Cal(&status_ctrl.PID_pitch, status_ctrl.expectedStatus.Pitch, 		status.Roll 	,	DMP_DATA.GYROx);
	PID_Postion_Cal(&status_ctrl.PID_roll,  status_ctrl.expectedStatus.Roll, 		status.Pitch	,	DMP_DATA.GYROy);
	PID_Postion_Cal(&status_ctrl.PID_yaw,	status_ctrl.expectedStatus.Yaw, 		status.Yaw		,	DMP_DATA.GYROz);
	PID_Postion_Cal(&status_ctrl.PID_alt,	status_ctrl.expectedStatus.Altitude , 	status.Altitude	,	2e38);

	PID_Limiter(&status_ctrl.PID_yaw.Output, status_ctrl.PID_OutputLimit);
	PID_Limiter(&status_ctrl.PID_alt.Output, status_ctrl.PID_OutputLimit);
	
	Pitch = status_ctrl.PID_pitch.Output;
	Roll  = status_ctrl.PID_roll.Output;
	Yaw   = status_ctrl.PID_yaw.Output; 
	Alt   = status_ctrl.PID_alt.Output; 
	
	//If the altitude value has problem...
	if (status.Altitude < 5) Alt = 0;
	
	Alt  += status_ctrl.Thro;
	
//	//X copter
//	status_ctrl.Motor_Out[1] = (int16_t)(Thro + Pitch -Roll - Yaw );    //M1  
//	status_ctrl.Motor_Out[2] = (int16_t)(Thro + Pitch +Roll + Yaw );    //M2 
//	status_ctrl.Motor_Out[3] = (int16_t)(Thro - Pitch +Roll - Yaw );    //M3  
//	status_ctrl.Motor_Out[0] = (int16_t)(Thro - Pitch -Roll + Yaw );    //M4

	
	//+ copter
	status_ctrl.Motor_Out[0] = (int16_t)(Alt - Roll - Yaw );
	status_ctrl.Motor_Out[1] = (int16_t)(Alt - Pitch  + Yaw );  
	status_ctrl.Motor_Out[2] = (int16_t)(Alt + Roll - Yaw );   // P调 1和3
	status_ctrl.Motor_Out[3] = (int16_t)(Alt + Pitch  + Yaw );  //P 调 2和4
	
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
