#ifndef __ES__PIN__
#define __ES__PIN__
#include "sys.h"

/**
ES Pins Functions:

	ESP_Read(Pin)		//Read Pin status (0 or 1)
	ESP_Set(Pin)		//Set Pin Output to 1
	ESP_Set2(Value, Pin)//Set Pin Output to desired value (1 or 0)
	ESP_Reset(Pin)		//Set Pin Output to 0
	ESP_SetOut(Pin)		//Set Pin Mode to Output
	ESP_SetIn(Pin)		//Set Pin Mode to Input
	
 */

/////////////////////////////////
/// * ES Pins
/// For STM32, Write	A, 10	to present GPIOA.10
/// For Renesas, Write  3, 1	to present P3.1

/*----------------------------------------------*
 |		User Pin			Group	Port		|       */
#define Pin_MODE1 			D,		9
#define Pin_MODE2 			D,		10

#define Pin_IIC_SDA			B,		7
#define Pin_IIC_SCL			B,		6

#define Pin_CCD_SI			A,		0
#define Pin_CCD_CLK			A,		3

#define Pin_Laser_L			C,		6
#define Pin_Laser_M			C,		7
#define Pin_Laser_R			C,		9


#endif
