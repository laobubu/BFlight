#include "sys.h"
#include "AlgorithmBasic.h"
#include "Plan.h"

Plan_t plan;

void Plan_Init(void) {
	plan.currentPoint = 0;
	plan.pointCount = 0;
	plan.time_since = millis();
}

#define CURRENT (plan.points[plan.currentPoint])
#define NEXT    (plan.points[plan.currentPoint + 1])
void Plan_Process(void) {
	
}
