#include "sys.h"
#include "Copycat.h"

#include "StatusCalc.h"

static uint32_t lastIntegTime;
Copycat_t Copycat;

void CC_Init(void)
{
	
}

void CC_Start(void)
{
	Copycat.x = 0;
	Copycat.y = 0;
	lastIntegTime = 0;
}

void CC_Integrate(void)
{
	float dt;
	if (lastIntegTime) {
		dt = millis() - lastIntegTime;
		Copycat.x += dt * status.Roll;
		Copycat.y += dt * status.Pitch;
	}
	lastIntegTime = millis();
}
