#ifndef __Ultrasonic
#define __Ultrasonic

//超声波模块
//使用 Ultrasonic_Trig() 开始采样（需要20us）
//使用 Ultrasonic.altitude 读取距离，单位 cm
//使用 Ultrasonic.status 读取状态，为 USS_IDLE 表示空闲/采样完成
//
//记得将 Ultrasonic_Echo() 放到ECHO的中断函数里，传入值为ECHO电平数值

typedef enum {
	USS_IDLE = 0,
	USS_PREPARE,
	USS_TIMING
} Ultrasonic_Status;

typedef struct {
	Ultrasonic_Status status;
	float altitude;
} Ultrasonic_Typedef;

extern Ultrasonic_Typedef Ultrasonic;

void Ultrasonic_Init(void);
void Ultrasonic_Trig(void);
void Ultrasonic_Echo(char high);
void Ultrasonic_TimeoutCallback(void);

#endif
