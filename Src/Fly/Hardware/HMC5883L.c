#include "HMC5883L.h"
#include "IIC.h"
#include "string.h"
#include "stdio.h"

#include "Algorithm/AlgorithmBasic.h"

#define HMC58X3_writeReg(addr, data) IICwriteByte(HMC58X3_ADDR, addr, data)

HMC58X3_TypeDef HMC58X3;
char HMC58X3_DRDY = 0;

void delay_us(unsigned short us);

void HMC58X3_setMode(unsigned char mode);
void HMC58X3_setDOR(unsigned char DOR);
void HMC58X3_FIFO_init(void);
void HMC58X3_newValues(int16_t x,int16_t y,int16_t z);
void HMC58X3_getlastValues(int16_t *x,int16_t *y,int16_t *z);

void HMC58X3_Init(void) {
	
	char id[3];
	id[0]=IICreadOneByte(HMC58X3_ADDR,HMC58X3_R_IDA);  
	id[1]=IICreadOneByte(HMC58X3_ADDR,HMC58X3_R_IDB);
	id[2]=IICreadOneByte(HMC58X3_ADDR,HMC58X3_R_IDC);
	
	//HMC的固定ID号为三个字节，16进制表示分别为48,34,33
	if (!((id[0]==0x48)&&(id[1]==0x34)&&(id[2]==0x33))) {
		printf("FAILED TO DETECT HMC58X3\r\n");
		while(1);
	}
	
	HMC58X3_writeReg(HMC58X3_R_CONFA, 0x70); // 8 samples averaged, 75Hz frequency, no artificial bias.
	HMC58X3_writeReg(HMC58X3_R_CONFB, 0xA0);
	HMC58X3_writeReg(HMC58X3_R_MODE,  0x00);
	
	HMC58X3_setMode(0);
	HMC58X3_setDOR(6);  //75hz 更新率
	HMC58X3_FIFO_init();
}

void HMC58X3_setMode(unsigned char mode) {
	if (mode > 2) return;
	HMC58X3_writeReg(HMC58X3_R_MODE, mode);
	delay_us(100);
}

void HMC58X3_setDOR(unsigned char DOR) {
	if (DOR>6) return;
	HMC58X3_writeReg(HMC58X3_R_CONFA,DOR<<2);
}

void HMC58X3_FIFO_init(void) {
	unsigned char i;
	memset(&HMC58X3, 0, sizeof(HMC58X3_TypeDef));
	for(i=0;i<20;i++){	//连续读取100次数据，以初始化FIFO数组
		HMC58X3_ReadSensor();
		delay_ms(1);  //延时再读取数据
	}
}

void HMC58X3_setGain(unsigned char gain) { 
  // 0-7, 1 default
  if (gain > 7) return;
  HMC58X3_writeReg(HMC58X3_R_CONFB, gain << 5);
}

void HMC58X3_ReadSensor(void) {
   unsigned char vbuff[6];
   IICreadBytes(HMC58X3_ADDR,HMC58X3_R_XM,6,vbuff);
   HMC58X3_newValues(((int16_t)vbuff[0] << 8) | vbuff[1],((int16_t)vbuff[4] << 8) | vbuff[5],((int16_t)vbuff[2] << 8) | vbuff[3]);
}

/**************************实现函数********************************************
*函数原型:	   void  HMC58X3_newValues(int16_t x,int16_t y,int16_t z)
*功　　能:	   更新一组数据到FIFO数组并刷新滤波输出
输入参数：  磁力计三个轴对应的ADC值
输出参数：  无
*******************************************************************************/
void  HMC58X3_newValues(int16_t x,int16_t y,int16_t z)
{
	/*
	HMC58X3._FIFOSum[0] -= HMC58X3._FIFO[HMC58X3._FIFOIndex][0];
	HMC58X3._FIFOSum[1] -= HMC58X3._FIFO[HMC58X3._FIFOIndex][1];
	HMC58X3._FIFOSum[2] -= HMC58X3._FIFO[HMC58X3._FIFOIndex][2];
	HMC58X3._FIFOSum[0] += HMC58X3._FIFO[HMC58X3._FIFOIndex][0] = x;
	HMC58X3._FIFOSum[1] += HMC58X3._FIFO[HMC58X3._FIFOIndex][1] = y;
	HMC58X3._FIFOSum[2] += HMC58X3._FIFO[HMC58X3._FIFOIndex][2] = z;
	if (++HMC58X3._FIFOIndex >= HMC58X3_LENGTH) HMC58X3._FIFOIndex = 0;
	HMC58X3.X = HMC58X3._FIFOSum[0] / HMC58X3_LENGTH;
	HMC58X3.Y = HMC58X3._FIFOSum[1] / HMC58X3_LENGTH;
	HMC58X3.Z = HMC58X3._FIFOSum[2] / HMC58X3_LENGTH;
	*/
	#define LUCKY 0.7
	HMC58X3.X = HMC58X3.X * (1-LUCKY) + LUCKY * x;
	HMC58X3.Y = HMC58X3.Y * (1-LUCKY) + LUCKY * y;
	HMC58X3.Z = HMC58X3.Z * (1-LUCKY) + LUCKY * z;
	
	HMC58X3.Yaw = atan2((double)HMC58X3.Y,(double)HMC58X3.X) * (180 / 3.14159265);
}
