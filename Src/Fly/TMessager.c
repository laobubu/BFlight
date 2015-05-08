#include "TMessager.h"
#include "stdio.h"
#include "string.h"
#include "stm32f1xx_hal.h"
#include "DataPacker.h"

extern UART_HandleTypeDef huart1;

#define messager_buffer_length 256
static uint8_t messager_buffer[messager_buffer_length];
static uint8_t message_recv_pack[100];

struct pt ptMessagerThread;
PT_THREAD(TMessagerThread(struct pt *pt));

void Init_MessagerThread(void) {
	PT_INIT(&ptMessagerThread);
	
	//Add your init code here
	//设置 DMA 为 circular 模式
	
	HAL_UART_Receive_DMA(&huart1, messager_buffer, messager_buffer_length);
}

void Do_MessagerThread(void) {
	TMessagerThread(&ptMessagerThread);
}

static int32_t DMATransmitted = 0;
static int32_t DMATransmitted_Base = 0; //由中断来增大，每完成一圈加 messager_buffer_length 
static int32_t BufferSolveIndex = 0;	//当前解析位置
static int32_t BufferBegin = 0;
static enum {
	BS_NOTBEGAN = 0,		//未找到包头
	BS_BEGAN				//已收到包头，正在等待尾
} BufferStatus = BS_NOTBEGAN;
static uint8_t *tmp;

#define BufferReadByte(x)   messager_buffer[(x+messager_buffer_length)%messager_buffer_length]

PT_THREAD(TMessagerThread(struct pt *pt)) {
	PT_BEGIN(pt);
	
	while(1) {
		DMATransmitted = DMATransmitted_Base + messager_buffer_length - DMA1_Channel5->CNDTR;
		while (BufferSolveIndex <= DMATransmitted) {
			if (BufferStatus == BS_NOTBEGAN) {
				if  (
						BufferReadByte(BufferSolveIndex - 1) 	== 0xAA/* &&
						BufferReadByte(BufferSolveIndex) 		== 0xBB*/
					) 
				{	//收到包头
					BufferBegin = BufferSolveIndex - 1;
					BufferStatus = BS_BEGAN;
				}
			} else if (BufferStatus == BS_BEGAN) {
				if ( 
					(BufferReadByte(BufferBegin+1) == 0xBB && (BufferSolveIndex - BufferBegin + 1) == 32) ||	//Crazepony 格式，0xAA 0xBB 32byte 一个包 
					(BufferReadByte(BufferBegin+1) == 0xCC &&  BufferReadByte(BufferSolveIndex) == 0x00) ||		//我们自己的格式，0xAA 0xCC 开头，0x00结束
					((BufferSolveIndex - BufferBegin + 1) == sizeof(message_recv_pack))							//出意外了
				)
				{
					tmp = message_recv_pack;
					for (; BufferBegin <= BufferSolveIndex; BufferBegin++) {
						*tmp++ = BufferReadByte(BufferBegin);
					}
					DataPacker_ProcessRecvPack(message_recv_pack);
					BufferStatus = BS_NOTBEGAN;
				}
			}
			
			BufferSolveIndex++;
		}
		PT_YIELD(pt);
	}
	
	PT_END(pt);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	DMATransmitted_Base += messager_buffer_length;
}
