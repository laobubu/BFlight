#include "Laser.h"
#include "FlyBasic.h"

void Laser_Init(void)
{
	//GPIO Init
}

char Laser_Read(Laser_enum_t laser)
{
	switch (laser) {
		case LASER_LEFT:	return ESP_Read(Pin_Laser_L);
		case LASER_MIDDLE:	return ESP_Read(Pin_Laser_M);
		case LASER_RIGHT:	return ESP_Read(Pin_Laser_R);
	}
	return 0;
}
