#include "Laser.h"
#include "FlyBasic.h"

void Laser_Init(void)
{
	//GPIO Init
}

char Laser_Read(Laser_enum_t laser)
{
	switch (laser) {
		case LASER_LEFT:	return PFin(0);
		case LASER_RIGHT:	return PFin(1);
	}
	return 0;
}
