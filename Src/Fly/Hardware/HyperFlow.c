#include "FlyBasic.h"
#include "HyperFlow.h"
#include "stm32f4xx_hal.h"

HyperFlow_t HyperFlow;
extern UART_HandleTypeDef huart2;

static char temp123;

void HyperFlow_Init(void)
{
	//GPIO Init
	
	__HAL_UART_ENABLE(&huart2);
	USART2->CR1 |= USART_CR1_RXNEIE | USART_CR1_RE;
	
	HAL_UART_Receive_IT(&huart2, &temp123, 1);
	//HAL_UART_Receive(&huart2,&temp123,8,1);
}	


static uint8_t HC_hasNewData = 0;
uint8_t HyperFlow_HasNewData(void) {
	if (HC_hasNewData) {
		HC_hasNewData = 0;
		return 1;
	}
	return 0;
}


//收到的数据

volatile uint8_t HC_BytesLeft1 = 0;
volatile uint8_t* HC_BBA1;
volatile HyperFlow_t HyperFlow_DataGot;

void HyperFlow_Feed(uint8_t ch)
{
	if (HC_BytesLeft1 == 0) {
		if (ch == 0xAA) {
			HC_BytesLeft1 = sizeof(HyperFlow_DataGot)+1;
		}
	} else if (HC_BytesLeft1 == (sizeof(HyperFlow_DataGot)+1)) {
		if (ch == 0xFF) {
			HC_BytesLeft1--;
			HC_BBA1 = ( uint8_t*) &HyperFlow_DataGot;
		} else {
			HC_BytesLeft1 = 0;
		}
	} else {
		*(HC_BBA1++) = ch;
		if (!(--HC_BytesLeft1)) {
//			HyperCCD.nav_position = HyperCCD_DataGot.nav_position;
//			HyperCCD.run_out_of_line = HyperCCD_DataGot.run_out_of_line;
//			HyperCCD.turn_left = HyperCCD_DataGot.turn_left;
//			HyperCCD.turn_right = HyperCCD_DataGot.turn_right;
//			HyperCCD.time = HyperCCD_DataGot.time;
				HyperFlow.x = HyperFlow_DataGot.x ;
			  HyperFlow.y = HyperFlow_DataGot.y;
			  HC_hasNewData = 1;
		}
	}
}
