#include "Param.h"
#include <string.h>

Param_t Param;

void Param_Reset(void)
{
	memset(&Param, 0, sizeof(Param));
	
	//系统初始参数
	Param.RFix = -0.0;
	Param.PFix =  0.0 ; 
	Param.YFix = -0.0;
	Param.BOff =  0.0;
	Param.Mode =  0;
}
