#ifndef _OLED_H_
#define _OLED_H_

#include "sys.h"
#include "stdio.h"


/*********************LCD 设置坐标************************************/
extern void OLED_Set_Pos(unsigned char x, unsigned char y); 

/*********************LCD全屏************************************/
extern void OLED_Fill(unsigned char bmp_dat);

/*********************LCD复位************************************/
extern void OLED_CLS(void);

/*********************LCD初始化************************************/
extern void OLED_Init(void);

/***************功能描述：显示6*8一组标准ASCII字符串	显示的坐标（x,y），y为页范围0～7****************/
extern void OLED_P6x8Str(unsigned char x,unsigned char y,char ch[]);

/***************功能描述：反色显示6*8一组标准ASCII字符串	显示的坐标（x,y），y为页范围0～7****************/
extern void INV_OLED_P6x8Str(unsigned char x,unsigned char y,char ch[]);

/*******************功能描述：显示8*16一组标准ASCII字符串	 显示的坐标（x,y），y为页范围0～7****************/
extern void OLED_P8x16Str(unsigned char x,unsigned char y,char ch[]);

/*****************功能描述：显示16*16点阵  显示的坐标（x,y），y为页范围0～7****************************/
extern void OLED_P16x16Ch(unsigned char x,unsigned char y,unsigned char N);

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
extern void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,const unsigned char BMP[]);
/**********写一个字符***********/
extern void OLED_write_char(unsigned char c);

extern void OLED_write_5_num(unsigned char X,unsigned char Y,signed long s);
extern void OLED_write_3_num(unsigned char X,unsigned char Y,signed int s);
extern void OLED_write_2_num(unsigned char X,unsigned char Y,signed int s);
extern void OLED_write_1_num(unsigned char X,unsigned char Y,signed int s);
//写double型的数据，小数点后两位，小数点前三位
extern void OLED_write_float2(unsigned char X,unsigned char Y,float s);
//写float型的数据，小数点后三位，小数点前两位
extern void OLED_write_float3(unsigned char X,unsigned char Y,float s);

extern char OLED_args[];

/***********使用 printf 写文字到指定坐标**********/
// 例如 OLED_P6x8Printf(0,0, "Hello World");
// 下面 OLED_P6x8PrintfINV 意思是反色显示
#define OLED_P6x8Printf(x,y,...)  	 {sprintf(OLED_args,__VA_ARGS__); OLED_P6x8Str(x,y,OLED_args);}
#define OLED_P6x8PrintfINV(x,y,...)  {sprintf(OLED_args,__VA_ARGS__); INV_OLED_P6x8Str(x,y,OLED_args);}


#endif
