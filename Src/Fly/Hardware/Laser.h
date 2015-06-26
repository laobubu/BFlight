#ifndef __LASER_H_
#define __LASER_H_

typedef enum {
	LASER_LEFT,
	LASER_RIGHT
} Laser_enum_t;

void Laser_Init(void);
char Laser_Read(Laser_enum_t laser);

#endif
