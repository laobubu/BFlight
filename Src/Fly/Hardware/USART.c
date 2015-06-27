#include "USART.h"
#include "FlyBasic.h"

#include <stdarg.h>

/*
//Designed for FreeRTOS

#include "cmsis_os.h"
osSemaphoreId sCameraRequiredHandle;

void patchedPrintf_Init(void) {
	osSemaphoreDef(sCameraRequired);
	sCameraRequiredHandle = osSemaphoreCreate(osSemaphore(sCameraRequired), 1);
}

int patchedPrintf(const char* fmt,...) {
	static va_list argptr;
	static int rtn;
	osSemaphoreWait(sCameraRequiredHandle, osWaitForever);
	va_start(argptr, fmt);
	rtn = vprintf(fmt,argptr);
	va_end(argptr);
	osSemaphoreRelease(sCameraRequiredHandle);
	return rtn;
}
*/


#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)	  
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
	while((USART1->SR&0X40)==0); //Send till fin.  
    USART1->DR = (unsigned char) ch; 
	return ch;
}
