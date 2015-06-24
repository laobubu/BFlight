#ifndef __px4flow_H_
#define __px4flow_H_

#include <stdint.h>

typedef struct {
	float x,y;
	float ratio;	//ratio
	uint64_t lastUpdate;
} PX4Flow_Typedef;

extern PX4Flow_Typedef PX4Flow;

void PX4Flow_Init(void);
void PX4Flow_FeedByte(char byte);

#endif
