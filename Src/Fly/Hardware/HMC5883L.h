#ifndef __HMC5883L_H_
#define __HMC5883L_H_

// HMC5883L 驱动程序

#include "stdint.h"
#include "stm32f1xx_hal.h"

#define HMC58X3_LENGTH 4

typedef struct{
	int16_t X,Y,Z;
	int16_t _FIFO[HMC58X3_LENGTH][3];
	uint8_t _FIFOIndex;
	int32_t _FIFOSum[3];
} HMC58X3_TypeDef;

extern HMC58X3_TypeDef HMC58X3;

extern char HMC58X3_DRDY;

//#define HMC58X3_IsDRDY()		(EXTI->PR & (1<<8))
//#define HMC58X3_ClearDRDY()		 EXTI->PR = (1<<8)
#define HMC58X3_IsDRDY()		(HMC58X3_DRDY)
#define HMC58X3_ClearDRDY()		HMC58X3_DRDY=0
void HMC58X3_Init(void);
void HMC58X3_setGain(unsigned char gain);
void HMC58X3_ReadSensor(void);



#define HMC58X3_ADDR 0x3C

#define HMC_POS_BIAS 1
#define HMC_NEG_BIAS 2
// HMC58X3 register map. For details see HMC58X3 datasheet
#define HMC58X3_R_CONFA 0
#define HMC58X3_R_CONFB 1
#define HMC58X3_R_MODE 2
#define HMC58X3_R_XM 3
#define HMC58X3_R_XL 4

#define HMC58X3_R_YM (7)  //!< Register address for YM.
#define HMC58X3_R_YL (8)  //!< Register address for YL.
#define HMC58X3_R_ZM (5)  //!< Register address for ZM.
#define HMC58X3_R_ZL (6)  //!< Register address for ZL.

#define HMC58X3_R_STATUS 9
#define HMC58X3_R_IDA 10
#define HMC58X3_R_IDB 11
#define HMC58X3_R_IDC 12

#endif
