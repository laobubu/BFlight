#include "FlyBasic.h"
#include "oled.h"
#include "IIC.h"

#include "oled_codetable.h"

#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel	    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xCF 
#define X_WIDTH 128
#define Y_WIDTH 64

#define I2C_OLED    0x78

#define OLED_WrCmd(Cmd)		IICwriteByte(I2C_OLED, 0X00, Cmd)
#define OLED_WrDat(Data)	IICwriteByte(I2C_OLED, 0X40, Data)
#define OLED_DLY_ms(x)		delay_ms(x)

/*********************LCD 设置坐标************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WrCmd(0xb0 + y);
	OLED_WrCmd( ( ( x & 0xf0 ) >> 4 ) | 0x10 );
	OLED_WrCmd( ( x & 0x0f ) | 0x01 ); 
} 
/*********************LCD全屏************************************/
void OLED_Fill(unsigned char bmp_dat) 
{
	unsigned char y,x;
	for ( y = 0 ; y < 8 ; y ++ )
	{
		OLED_WrCmd(0xb0 + y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for ( x = 0 ; x < X_WIDTH ; x ++ )
			OLED_WrDat(bmp_dat);
	}
}
/*********************LCD复位************************************/
void OLED_CLS(void)
{
	unsigned char y,x;	
	for ( y = 0 ; y < 8 ; y ++ )
	{
		OLED_WrCmd(0xb0 + y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10); 
		for ( x = 0 ; x < X_WIDTH ; x ++ )
			OLED_WrDat(0);
	}
}
/*********************LCD初始化************************************/
void OLED_Init(void)     
{
	OLED_DLY_ms(1);
	OLED_WrCmd(0xae);//--turn off oled panel
	OLED_WrCmd(0x00);//---set low column address
	OLED_WrCmd(0x10);//---set high column address
	OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WrCmd(0x81);//--set contrast control register
	OLED_WrCmd(0xcf); // Set SEG Output Current Brightness
	OLED_WrCmd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WrCmd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WrCmd(0xa6);//--set normal display
	OLED_WrCmd(0xa8);//--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3f);//--1/64 duty
	OLED_WrCmd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x00);//-not offset
	OLED_WrCmd(0xd5);//--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WrCmd(0xd9);//--set pre-charge period
	OLED_WrCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WrCmd(0xda);//--set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb);//--set vcomh
	OLED_WrCmd(0x40);//Set VCOM Deselect Level
	OLED_WrCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x02);//
	OLED_WrCmd(0x8d);//--set Charge Pump enable/disable
	OLED_WrCmd(0x14);//--set(0x10) disable
	OLED_WrCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
	OLED_WrCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
	OLED_WrCmd(0xaf);//--turn on oled panel
	OLED_Fill(0x00);  //初始清屏
	OLED_Set_Pos(0,0); 	
} 
/***************功能描述：显示6*8一组标准ASCII字符串	显示的坐标（x,y），y为页范围0～7****************/
void OLED_P6x8Str(unsigned char x,unsigned char y,char ch[])
{
	unsigned char c = 0,i = 0,j = 0;      
	while ( ch[j] != '\0' )
	{    
		c = ch[j] - 32;
		if ( x > 126 ) 
			{
				x = 0;
				y ++;
			}
		OLED_Set_Pos(x,y);    
		for ( i = 0 ; i < 6 ; i++ )     
			OLED_WrDat(F6x8[c][i]);  
		x += 6;
		j ++;
	}
}
/***************功能描述：反色显示6*8一组标准ASCII字符串	显示的坐标（x,y），y为页范围0～7****************/
void INV_OLED_P6x8Str(unsigned char x,unsigned char y,char ch[])
{
	unsigned char c = 0,i = 0,j = 0;      
	while ( ch[j] != '\0' )
	{    
		c = ch[j] - 32;
		if ( x > 126 ) 
			{
				x = 0;
				y ++;
			}
		OLED_Set_Pos(x,y);    
		for ( i = 0 ; i < 6 ; i++ )     
			OLED_WrDat(~F6x8[c][i]);  
		x += 6;
		j ++;
	}
}
/*******************功能描述：显示8*16一组标准ASCII字符串	 显示的坐标（x,y），y为页范围0～7****************/
void OLED_P8x16Str(unsigned char x,unsigned char y,char ch[])
{
	unsigned char c = 0 ,i = 0,j = 0;
	while ( ch[j] != '\0' )
	{    
		c = ch[j] - 32;
		if ( x > 120 )
			{
				x = 0;
				y ++;
			}
		OLED_Set_Pos(x,y);    
		for ( i = 0 ; i < 8 ; i++ )     
			OLED_WrDat(F8X16[c * 16 + i]);
		OLED_Set_Pos(x,y + 1);    
		for ( i = 0 ; i < 8 ; i ++)     
			OLED_WrDat(F8X16[c * 16 + i + 8]);  
		x += 8;
		j ++;
	}
}
/*****************功能描述：显示16*16点阵  显示的坐标（x,y），y为页范围0～7****************************/
void OLED_P16x16Ch(unsigned char x,unsigned char y,unsigned char N)
{
	unsigned char wm = 0;
	unsigned int adder = 32 * N;  //  	
	OLED_Set_Pos(x , y);
	for ( wm = 0 ; wm < 16 ; wm ++ )  //             
	{
		OLED_WrDat(F16x16[adder]);	
		adder += 1;
	}      
	OLED_Set_Pos(x,y + 1); 
	for ( wm = 0;wm < 16 ; wm ++ ) //         
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	} 	  	
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void Draw_BMP(unsigned char x0, unsigned char y0,unsigned char x1,unsigned char y1,const unsigned char BMP[])
{ 	
 unsigned int j = 0;
 unsigned char x,y;
	for ( y = y0 ; y < y1 ; y ++ )
	{
		OLED_Set_Pos(x0,y);				
	    for ( x = x0 ; x < x1 ; x ++ )
	    {      
	    	OLED_WrDat(BMP[j++]);	    	
	    }
	}
} 

/**********写一个字符***********/
void OLED_write_char(unsigned char c)
  {
    unsigned char line;

    c -= 32;

    for ( line = 0; line < 6 ; line ++)
    	OLED_WrDat(F6x8[c][line]);
  }
/*******写数字********/
void OLED_write_3_num(unsigned char X,unsigned char Y,signed int s)
{   
	 OLED_Set_Pos(X,Y);
	 if(s<0)
	 {
	 	s=0-s;
	 	OLED_write_char('-');
	 }
	 else
	 {
	   	OLED_write_char(' ');	
	 }
	 OLED_write_char((s/100)+0x30);
	 OLED_write_char(((s%100)/10)+0x30);
	 OLED_write_char((s%10)+0x30);	 
}

void OLED_write_5_num(unsigned char X,unsigned char Y,signed long s)
{   
	 OLED_Set_Pos(X,Y);
	 if(s<0)
	 {
	 	s=0-s;
	 	OLED_write_char('-');
	 }
	 else
	 {
	   	OLED_write_char(' ');	
	 }
	 OLED_write_char((s/10000)+0x30);
	 s %= 10000;
	 OLED_write_char((s/1000)+0x30);
	 s %= 1000;
	 OLED_write_char((s/100)+0x30);
	 OLED_write_char(((s%100)/10)+0x30);
	 OLED_write_char((s%10)+0x30);	 
}
  
  void OLED_write_2_num(unsigned char X,unsigned char Y,signed int s)
  {
     OLED_Set_Pos(X,Y);
	 if(s<0)
	 {
	 	s=0-s;
	 	OLED_write_char('-');
	 }
	 else
	 {
	   	OLED_write_char(' ');	
	 }
	 if((s/10)!=0)
	 OLED_write_char((s/10)+0x30);
	 OLED_write_char((s%10)+0x30);	 
  }


    
  void OLED_write_1_num(unsigned char X,unsigned char Y,signed int s)
  {
     OLED_Set_Pos(X,Y);
	  if(s<0)
	 {
	 	s=0-s;
	 	OLED_write_char('-');
	 }
	 else
	 {
	   	OLED_write_char(' ');	
	 }
	 OLED_write_char(s+0x30);	 
  }
  
  //写double型的数据，小数点后两位，小数点前两位
  void OLED_write_float2(unsigned char X,unsigned char Y,float s)
  {
     unsigned int num;
     
     OLED_Set_Pos(X,Y); 
     if (s < 0)//判断正负
     {
         s = 0 - s;
         OLED_write_char('-');
     }
     else 
     {
         OLED_write_char(' ');
     }
     num = (unsigned int)(s * 100);
	 if((num/10000)!=0)
	 OLED_write_char((num/10000)+0x30);//百位
	 if(((num/1000)!=0&&(num/10000)==0)||(num/10000)!=0)
     OLED_write_char(((num-num/10000*10000)/1000)+0x30);//十位
     num = num % 1000;
     OLED_write_char((num/100)+0x30);//个位
     num = num % 100;
     OLED_write_char('.');
     OLED_write_char((num/10)+0x30);//小数点后一位
     num = num % 10;
     OLED_write_char((num)+0x30);//小数点后第二位  
  }

   //写float型的数据，小数点后三位，小数点前两位
  void OLED_write_float3(unsigned char X,unsigned char Y,float s)
  {
     unsigned int num;
     
     OLED_Set_Pos(X,Y); 
     if (s < 0)//判断正负
     {
         s = 0 - s;
         OLED_write_char('-');
     }
     else 
     {
         OLED_write_char(' ');			
     }
     num = (unsigned int)(s * 1000);
	 if((num/10000)!=0)
	 OLED_write_char((num/10000)+0x30);//十位
	 num=num%10000;
	 if((num/1000)!=0)
     OLED_write_char((num/1000)+0x30);//个位
     num = num % 1000;
	 OLED_write_char('.');
     OLED_write_char((num/100)+0x30);//小数点后一位
     num = num % 100;
     OLED_write_char((num/10)+0x30);//小数点后两位
     num = num % 10;
     OLED_write_char((num)+0x30);//小数点后第三位  
  }

char OLED_args[60];
