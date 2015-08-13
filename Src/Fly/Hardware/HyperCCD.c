#include "FlyBasic.h"
#include "HyperCCD.h"
#include "stm32f4xx_hal.h"

HyperCCD_t HyperCCD;
extern UART_HandleTypeDef huart3;

static char temp123;

void HyperCCD_Init(void)
{
	//GPIO Init
	
	__HAL_UART_ENABLE(&huart3);
	USART3->CR1 |= USART_CR1_RXNEIE | USART_CR1_RE;
	
	HAL_UART_Receive_IT(&huart3, &temp123, 1);
}	


static uint8_t HC_hasNewData = 0;
uint8_t HyperCCD_HasNewData(void) {
	if (HC_hasNewData) {
		HC_hasNewData = 0;
		return 1;
	}
	return 0;
}


//收到的数据

volatile uint16_t HC_BytesLeft = 0;
volatile uint8_t* HC_BBA;
volatile HyperCCD_t HyperCCD_DataGot;

void HyperCCD_Feed(uint8_t ch)
{
	if (HC_BytesLeft == 0) {
		if (ch == 0xAA) {
			HC_BytesLeft = sizeof(HyperCCD_DataGot)+1;
		}
	} else if (HC_BytesLeft == (sizeof(HyperCCD_DataGot)+1)) {
		if (ch == 0xFF) {
			HC_BytesLeft--;
			HC_BBA = (uint8_t*) &HyperCCD_DataGot;
		} else {
			HC_BytesLeft = 0;
		}
	} else {
		*(HC_BBA++) = ch;
		if (!(--HC_BytesLeft)) {
			HyperCCD = HyperCCD_DataGot; 
			HC_hasNewData = 1;
		}
	}
}
