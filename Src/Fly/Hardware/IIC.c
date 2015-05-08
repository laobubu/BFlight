#include "IIC.h"
#include "stm32f1xx_hal.h"

#define IIC_SCL_1	GPIOB->BSRR = 1<<6
#define IIC_SCL_0	GPIOB->BRR  = 1<<6
#define IIC_SDA_1	GPIOB->BSRR = 1<<7
#define IIC_SDA_0	GPIOB->BRR  = 1<<7
#define READ_SDA	(GPIOB->IDR & (1<<7))

////驱动接口，GPIO模拟IIC
//SCL-->PB6
//SDA-->PB7
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0x80000000;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0x30000000;}

extern I2C_HandleTypeDef hi2c1;

void delay_us(unsigned short us) {
	unsigned char lnc = 20 // SystemCoreClock / 1000000
		, t;
	while(us--) {
		t = lnc;
		while(--t) ;
	}
}

void IICinit(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	HAL_I2C_DeInit(&hi2c1);
	
	__GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**************************实现函数********************************************
*函数原型:		void IIC_Start(void)
*功　　能:		产生IIC起始信号
*******************************************************************************/
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出 
	IIC_SDA_1;	  	  
	IIC_SCL_1;
	delay_us(4);
 	IIC_SDA_0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL_0;//钳住I2C总线，准备发送或接收数据 
}

/**************************实现函数********************************************
*函数原型:		void IIC_Stop(void)
*功　　能:	    //产生IIC停止信号
*******************************************************************************/	  
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL_0;
	IIC_SDA_0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL_1; 
	IIC_SDA_1;//发送I2C总线结束信号
	delay_us(4);							   	
}

/**************************实现函数********************************************
*函数原型:		u8 IIC_Wait_Ack(void)
*功　　能:	    等待应答信号到来 
//返回值：1，接收应答失败
//        0，接收应答成功
*******************************************************************************/
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA_1;delay_us(1);	   
	IIC_SCL_1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>50)
		{
			IIC_Stop();
			return 1;
		}
	  delay_us(1);
	}
	IIC_SCL_0;//时钟输出0 	   
	return 0;  
} 

/**************************实现函数********************************************
*函数原型:		void IIC_Ack(void)
*功　　能:	    产生ACK应答
*******************************************************************************/
void IIC_Ack(void)
{
	IIC_SCL_0;
	SDA_OUT();
	IIC_SDA_0;
	delay_us(1);
	IIC_SCL_1;
	delay_us(1);
	IIC_SCL_0;
}
	
/**************************实现函数********************************************
*函数原型:		void IIC_NAck(void)
*功　　能:	    产生NACK应答
*******************************************************************************/	    
void IIC_NAck(void)
{
	IIC_SCL_0;
	SDA_OUT();
	IIC_SDA_1;
	delay_us(1);
	IIC_SCL_1;
	delay_us(1);
	IIC_SCL_0;
}					 				     

/**************************实现函数********************************************
*函数原型:		void IIC_Send_Byte(u8 txd)
*功　　能:	    IIC发送一个字节
*******************************************************************************/		  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL_0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {
		if ((txd&0x80)>>7)
			IIC_SDA_1;
		else
			IIC_SDA_0;
        txd<<=1; 	  
		delay_us(1);   
		IIC_SCL_1;
		delay_us(1); 
		IIC_SCL_0;	
		delay_us(1);
    }	 
} 	 
   
/**************************实现函数********************************************
*函数原型:		u8 IIC_Read_Byte(unsigned char ack)
*功　　能:	    //读1个字节，ack=1时，发送ACK，ack=0，发送nACK 
*******************************************************************************/  
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL_0; 
        delay_us(1);
		IIC_SCL_1;
        receive<<=1;
        if (READ_SDA) receive++;
		delay_us(1); 
    }					 
    if (ack)
        IIC_Ack(); //发送ACK 
    else
        IIC_NAck();//发送nACK  
    return receive;
}

/**************************实现函数********************************************
*函数原型:		unsigned char IICreadOneByte(unsigned char I2C_Addr,unsigned char addr)
*功　　能:	    读取指定设备 指定寄存器的一个值
输入	I2C_Addr  目标设备地址
		addr	   寄存器地址
返回   读出来的值
*******************************************************************************/ 
unsigned char IICreadOneByte(unsigned char I2C_Addr,unsigned char addr)
{
	unsigned char res=0;
	
	IIC_Start();
  
	IIC_Send_Byte(I2C_Addr);	   //发送写命令
	res++;
	IIC_Wait_Ack();
	IIC_Send_Byte(addr); res++;  //发送地址
	IIC_Wait_Ack();	  
	//IIC_Stop();//产生一个停止条件	
	IIC_Start();
	IIC_Send_Byte(I2C_Addr+1); res++;          //进入接收模式			   
	IIC_Wait_Ack();
	res=IIC_Read_Byte(0);	   
    IIC_Stop();//产生一个停止条件

	return res;
}


/**************************实现函数********************************************
*函数原型:		u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data)
*功　　能:	    读取指定设备 指定寄存器的 length个值
输入	dev  目标设备地址
		reg	  寄存器地址
		length 要读的字节数
		*data  读出的数据将要存放的指针
返回   读出来的字节数量
*******************************************************************************/ 
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data){
    u8 count = 0;
	u8 temp;
	IIC_Start();
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //发送地址
    IIC_Wait_Ack();	  
	IIC_Start();
	IIC_Send_Byte(dev+1);  //进入接收模式	
	IIC_Wait_Ack();
	
    for(count=0;count<length;count++){
		 
		 if(count!=(length-1))
		 	temp = IIC_Read_Byte(1);  //带ACK的读数据
		 	else  
			temp = IIC_Read_Byte(0);	 //最后一个字节NACK

		data[count] = temp;
	}
    IIC_Stop();//产生一个停止条件
    return count;
}

/**************************实现函数********************************************
*函数原型:		u8 IICwriteOneByte(u8 dev, u8 reg, u8 length, u8* data)
*功　　能:	    将1个字节写入指定设备，参考 MS5611 的文档11页
输入	dev  目标设备地址
		data  将要写的数据
返回   返回是否成功
*******************************************************************************/ 
u8 IICwriteOneByte(u8 dev, u8 data){
	IIC_Start();
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(data);   //发送地址
    IIC_Wait_Ack();
	IIC_Stop();//产生一个停止条件

    return 1; //status == 0;
}

/**************************实现函数********************************************
*函数原型:		u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data)
*功　　能:	    将多个字节写入指定设备 指定寄存器
输入	dev  目标设备地址
		reg	  寄存器地址
		length 要写的字节数
		*data  将要写的数据的首地址
返回   返回是否成功
*******************************************************************************/ 
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data){
  
 	u8 count = 0;
	IIC_Start();
	IIC_Send_Byte(dev);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);   //发送地址
    IIC_Wait_Ack();	  
	for(count=0;count<length;count++){
		IIC_Send_Byte(data[count]); 
		IIC_Wait_Ack(); 
	 }
	IIC_Stop();//产生一个停止条件

    return 1; //status == 0;
}

/**************************实现函数********************************************
*函数原型:		u8 IICreadByte(u8 dev, u8 reg, u8 *data)
*功　　能:	    读取指定设备 指定寄存器的一个值
输入	dev  目标设备地址
		reg	   寄存器地址
		*data  读出的数据将要存放的地址
返回   1
*******************************************************************************/ 
u8 IICreadByte(u8 dev, u8 reg, u8 *data){
	*data=IICreadOneByte(dev, reg);
    return 1;
}

/**************************实现函数********************************************
*函数原型:		unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
*功　　能:	    写入指定设备 指定寄存器一个字节
输入	dev  目标设备地址
		reg	   寄存器地址
		data  将要写入的字节
返回   1
*******************************************************************************/ 
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data){
    return IICwriteBytes(dev, reg, 1, &data);
}

/**************************实现函数********************************************
*函数原型:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的多个位
输入	dev  目标设备地址
		reg	   寄存器地址
		bitStart  目标字节的起始位
		length   位长度
		data    存放改变目标字节位的值
返回   成功 为1 
 		失败为0
*******************************************************************************/ 
u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
{

    u8 b;
    if (IICreadByte(dev, reg, &b) != 0) {
        u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
        data <<= (8 - length);
        data >>= (7 - bitStart);
        b &= mask;
        b |= data;
        return IICwriteByte(dev, reg, b);
    } else {
        return 0;
    }
}

/**************************实现函数********************************************
*函数原型:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的1个位
输入	dev  目标设备地址
		reg	   寄存器地址
		bitNum  要修改目标字节的bitNum位
		data  为0 时，目标位将被清0 否则将被置位
返回   成功 为1 
 		   失败为0
*******************************************************************************/ 
u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data){
    u8 b;
    
    IICreadByte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    
    return IICwriteByte(dev, reg, b);
}


/*
//USING HARDWARE I2C
//NOTICE: INIT WAS SET IN STM32CUBE
extern I2C_HandleTypeDef hi2c1;
#define I2C_TIMEOUT HAL_MAX_DELAY

void IICinit(void){}

u8 IICreadOneByte(u8 dev,u8 reg) {
	uint8_t dd;
	while (HAL_BUSY != HAL_I2C_Mem_Read(&hi2c1, dev, reg, I2C_MEMADD_SIZE_8BIT, &dd, 1, I2C_TIMEOUT));
	return dd;
}

u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data) {
	HAL_StatusTypeDef rtn;
	while (HAL_BUSY != (rtn = HAL_I2C_Mem_Read(&hi2c1, dev, reg, I2C_MEMADD_SIZE_8BIT, data, length, I2C_TIMEOUT)));
	return rtn==HAL_OK;
}

u8 IICwriteByte(u8 dev, u8 reg, u8 data) {
	HAL_StatusTypeDef rtn;
	while (HAL_BUSY != (rtn = HAL_I2C_Mem_Write(&hi2c1, dev, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, I2C_TIMEOUT)));
	return rtn==HAL_OK;
}

u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data) {
	HAL_StatusTypeDef rtn;
	while (HAL_BUSY != (rtn = HAL_I2C_Mem_Write(&hi2c1, dev, reg, I2C_MEMADD_SIZE_8BIT, data, length, I2C_TIMEOUT)));
	return rtn==HAL_OK;
}

u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data) {
	u8 b = IICreadOneByte(dev,reg) ;
	u8 mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
	data <<= (8 - length);
	data >>= (7 - bitStart);
	b &= mask;
	b |= data;
	return IICwriteByte(dev, reg, b);
}

u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data){
	u8 b;
	b = IICreadOneByte(dev,reg) & (~(1<<bitNum));
	if (data) b |= 1<<bitNum;
	return IICwriteByte(dev, reg, b);
}
*/
