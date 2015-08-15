#ifndef __copycat_H__
#define __copycat_H__

typedef struct {
	float x,y;
} Copycat_t;

extern Copycat_t Copycat;

void CC_Init(void);
void CC_Start(void);
void CC_Integrate(void);

#endif
