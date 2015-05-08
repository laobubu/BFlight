#ifndef __IIC_H
#define __IIC_H

typedef unsigned char u8;

void IICinit(void);

u8 IICreadOneByte(u8 dev,u8 reg);
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data);

u8 IICwriteOneByte(u8 dev, u8 data);
u8 IICwriteByte(u8 dev, u8 reg, u8 data);
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data);
u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data);
u8 IICwriteBit(u8 dev,u8 reg,u8 bitNum,u8 data);

#endif
