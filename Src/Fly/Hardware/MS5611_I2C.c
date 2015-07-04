#include "FlyBasic.h"
#include "MS5611_I2C.h"
#include "IIC.h"
#include "math.h"

typedef union {
	uint32_t value;
	uint8_t bytes[4];
} uint32andUint8_t;		//便于 I2C 的读取

typedef union {
    uint16_t value;
    uint8_t bytes[2];
} uint16andUint8_t;

#define SQR(x) ((x)*(x))

#define MS5611_FILTER_RATION 0.7f

///////////////////////////////////////

//#define OSR  256  // 0.60 mSec conversion time (1666.67 Hz)
//#define OSR  512  // 1.17 mSec conversion time ( 854.70 Hz)
//#define OSR 1024  // 2.28 mSec conversion time ( 357.14 Hz)
#define OSR 2048  // 4.54 mSec conversion time ( 220.26 Hz)
//#define OSR 4096  // 9.04 mSec conversion time ( 110.62 Hz)

///////////////////////////////////////

/*
ms5611出厂时已经确定的6个系数：

	c1，气压敏感度，SENS
	c2，气压偏差值，OFF
	c3，气压敏感度的温度系数，简写为TCS
	c4，气压偏差值的温度系数，简写为TCO
	c5，参考温度，Tref
	c6，温度系数，TEMPSENS
*/
uint16andUint8_t c1, c2, c3, c4, c5, c6;

uint32andUint8_t d1;

uint32andUint8_t d2;

int32_t dT;

MS5611_TypeDef MS5611;

uint8_t newPressureReading = 0;
uint8_t newTemperatureReading = 0;

void pushNewPressure(uint32_t v) {/*
	static uint8_t curi = 0;
	static uint32_t sum[8] = {0,0,0,0,0,0,0,0};
	static uint64_t sum2 = 0;
	sum2 += v;
	sum2 -= sum[curi];
	sum[curi] = v;
	if (++curi == 8) curi = 0;
	MS5611.Pressure = sum2 / 8.0;*/
	
	MS5611.Pressure = MS5611.Pressure * 0.5f + 0.5f * v;
}

void requestPressure(void)
{
	#if   (OSR ==  256)
	IICwriteOneByte( MS5611Address, 0x40);  // Request pressure conversion
	#elif (OSR ==  512)
	IICwriteOneByte( MS5611Address, 0x42);
	#elif (OSR == 1024)
	IICwriteOneByte( MS5611Address, 0x44);
	#elif (OSR == 2048)
	IICwriteOneByte( MS5611Address, 0x46);
	#elif (OSR == 4096)
	IICwriteOneByte( MS5611Address, 0x48);
	#endif
}

void readPressure(void)
{
	uint8_t data[3];

	IICreadBytes( MS5611Address, 0x00, 3, data);    // Request pressure read

	d1.bytes[2] = data[0];
	d1.bytes[1] = data[1];
	d1.bytes[0] = data[2];
}

void requestTemperature(void)
{
	#if   (OSR ==  256)
	IICwriteOneByte( MS5611Address, 0x50);   // Request temperature converison
	#elif (OSR ==  512)
	IICwriteOneByte( MS5611Address, 0x52);
	#elif (OSR == 1024)
	IICwriteOneByte( MS5611Address, 0x54);
	#elif (OSR == 2048)
	IICwriteOneByte( MS5611Address, 0x56);
	#elif (OSR == 4096)
	IICwriteOneByte( MS5611Address, 0x58);
	#endif
}

void readTemperature(void)
{
	uint8_t data[3];
	IICreadBytes( MS5611Address, 0x00, 3, data);    // Request temperature read
	d2.bytes[2] = data[0];
	d2.bytes[1] = data[1];
	d2.bytes[0] = data[2];
}

///////////////////////////////////////////////////////////////////////////////
// Calculate Temperature
///////////////////////////////////////////////////////////////////////////////

void calculateTemperature(void)
{
	dT = d2.value - ((int32_t)c5.value << 8);
	MS5611.Temperature = 2000 + (int32_t)(((int64_t)dT * c6.value) >> 23);
}

///////////////////////////////////////////////////////////////////////////////
// Calculate Pressure Altitude
///////////////////////////////////////////////////////////////////////////////

void calculatePressureAltitude(void)
{
	int64_t offset;
	int64_t offset2 = 0;

	int64_t sensitivity;
	int64_t sensitivity2 = 0;

	int64_t f;

	int32_t ms5611Temp2  = 0;

	offset      = ((int64_t)c2.value << 16) + (((int64_t)c4.value * dT) >> 7);
	sensitivity = ((int64_t)c1.value << 15) + (((int64_t)c3.value * dT) >> 8);

	if (MS5611.Temperature < 2000)	//温度小于20摄氏度，需要二阶补偿
	{
		ms5611Temp2  = SQR(dT) >> 31;

		f	 		 = SQR(MS5611.Temperature - 2000);
		offset2      = 5 * f >> 1;
		sensitivity2 = 5 * f >> 2;

		if (MS5611.Temperature < -1500)
		{
			f 			  = SQR(MS5611.Temperature + 1500);
			offset2      +=  7 * f;
			sensitivity2 += 11 * f >> 1;
		}

		MS5611.Temperature -= ms5611Temp2;

		offset -= offset2;
		sensitivity -= sensitivity2;
	}

	pushNewPressure((((d1.value * sensitivity) >> 21) - offset) >> 15);

	float newalt = (44330.0f * (1.0f - pow((float)MS5611.Pressure / 101325.0f, 1.0f / 5.255f)));
	MS5611.Altitude = MS5611.Altitude * (1.0f-MS5611_FILTER_RATION) + MS5611_FILTER_RATION * newalt;
	MS5611.deltaAltitude = MS5611.Altitude - MS5611.floorAltitude;

	//DBG_PRINT("calculate Pressure Altitude : %f\n",pressureAlt50Hz);
}

///////////////////////////////////////////////////////////////////////////////
// Pressure Initialization
///////////////////////////////////////////////////////////////////////////////

void MS5611_Init(void)
{
	uint8_t data[2];
	
	MS5611.Temperature = 0;
	MS5611.Altitude = 0.0f;
	MS5611.floorAltitude = 0.0f;

	IICwriteOneByte( MS5611Address, 0x1E);      // Reset Device

	delay_ms(10);

	IICreadBytes( MS5611Address, 0xA2, 2, data);    // Read Calibration Data C1
	c1.bytes[1] = data[0];
	c1.bytes[0] = data[1];

	IICreadBytes( MS5611Address, 0xA4, 2, data);    // Read Calibration Data C2
	c2.bytes[1] = data[0];
	c2.bytes[0] = data[1];

	IICreadBytes( MS5611Address, 0xA6, 2, data);    // Read Calibration Data C3
	c3.bytes[1] = data[0];
	c3.bytes[0] = data[1];

	IICreadBytes( MS5611Address, 0xA8, 2, data);    // Read Calibration Data C4
	c4.bytes[1] = data[0];
	c4.bytes[0] = data[1];

	IICreadBytes( MS5611Address, 0xAA, 2, data);    // Read Calibration Data C5
	c5.bytes[1] = data[0];
	c5.bytes[0] = data[1];

	IICreadBytes( MS5611Address, 0xAC, 2, data);    // Read Calibration Data C6
	c6.bytes[1] = data[0];
	c6.bytes[0] = data[1];

	MS5611_Read();
}


//读取函数，务必5ms运行一次
void MS5611_Read(void) {	
	// 0 读温度
	// 1 读气压并计算
	static char LASTREAD = 0;
	if (LASTREAD == 0) {
		readTemperature();
		requestPressure();
		calculateTemperature();
		LASTREAD = 1;
	} else if (LASTREAD == 1) {
		readPressure();
		requestTemperature();
		calculatePressureAltitude();
		LASTREAD = 0;
	}
}

void MS5611_SetFloor(void) {
	MS5611.floorAltitude = MS5611.Altitude;
	MS5611.deltaAltitude = MS5611.Altitude - MS5611.floorAltitude;
}
