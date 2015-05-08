#ifndef __FILTER_H
#define __FILTER_H

typedef struct {
	float _cutoff_freq1; 
	float _a11;
	float _a21;
	float _b01;
	float _b11;
	float _b21;
	float _delay_element_11;        // buffered sample -1
	float _delay_element_21;        // buffered sample -2
} LPF2pFilter_TypeDef;

void  LPF2pSetCutoffFreq(LPF2pFilter_TypeDef *filterObject, float sample_freq, float cutoff_freq);
float LPF2pApply(LPF2pFilter_TypeDef *filterObject, float sample);

#endif
