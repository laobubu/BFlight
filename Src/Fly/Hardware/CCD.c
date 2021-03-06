#include "FlyBasic.h"
#include "CCD.h"

#define TSL1401_SI(val)		ESP_Set2(val, Pin_CCD_SI)
#define TSL1401_CLK(val)	ESP_Set2(val, Pin_CCD_CLK)
//#define TSL1401_Delay(val)	delay_us(10) //产生一段用于时钟信号的延时

static void TSL1401_Delay()
{
	volatile uint8_t dd = 0;
	while(dd!=20)
		dd++;
}

CCD_TypeDef CCD;
extern ADC_HandleTypeDef hadc1;

void CCD_Init(void) {
	//HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start(&hadc1);
}

void CCD_Sample(void) {
	u8 i;
	
	TSL1401_SI(0);
	TSL1401_CLK(0);
	
	TSL1401_Delay();
	
	TSL1401_SI(1);
	TSL1401_CLK(1);
	
	TSL1401_Delay();
	
	TSL1401_SI(0); 										//此时停止曝光
	TSL1401_Delay();									//等待A0电压稳定

	for(i=0; i<128; i++){
		TSL1401_CLK(0);									//下降沿 A0数据不变
		hadc1.Instance->DR;
		hadc1.Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
		while(!(hadc1.Instance->SR & ADC_FLAG_EOC));
		//HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY); //等待转换完成
		TSL1401_CLK(1);									//上升沿 将A0变成下一个数据
		CCD.data[i] = (u8)(HAL_ADC_GetValue(&hadc1)>>4);
		TSL1401_Delay();								//等待A0电压稳定
		TSL1401_Delay();								//等待A0电压稳定
		TSL1401_Delay();								//等待A0电压稳定
	}
	//HAL_ADC_Stop(&hadc1);

	TSL1401_CLK(0);
}
