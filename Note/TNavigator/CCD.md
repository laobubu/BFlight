相关头文件
===========================================
```
#include "Hardware/CCD.h"
```





用到的变量声明
===========================================
```
PID_Typedef  nav_pid;
其他忘记了
```






初始化代码
===========================================
```
PID_Init(&nav_pid, PID_MODE_DERIVATIV_CALC, 1.0f);
CCD_Init();
```





执行的代码
===========================================
```
忘记了

哦对了，有关曝光时间那啥的在别的文件（TCCD）里哈，曝光和TNavigator是同时进行的两条线。
这里只用管读取就是了
```



自定义函数
==========================================
```
void  nav_handle(){
	unsigned char i, pmax = 255, pmin = 255;
	signed short dmax = 0, dmin = 0, dtmp = 0;
	
	for(i=20;i<100;i++){
		dtmp = CCD.data[i] - CCD.data[i+1];
					if (dtmp > dmax) {
						dmax = dtmp;
						pmax = i;
					}
					if (dtmp < dmin) {
						dmin = dtmp;
						pmin = i;
					}
		CCD.data[i] = dtmp/2 + 127;
	}
	
	if (dmax - dmin > 15)	{
				nav_position = (pmax + pmin)/2;
				CCD.data[nav_position] = 0xFF;
		    run_out_of_line = 0;
  }
	else {
	      run_out_of_line = 1;
	   
	}
	
}
```
