#include "AvgFilter.h"
#include "AlgorithmBasic.h"

void LPF2pSetCutoffFreq_1(LPF2pFilter_TypeDef *filterObject, float sample_freq, float cutoff_freq)
{
	float fr =0;  
    float ohm =0;
    float c =0;
	
	fr= sample_freq/cutoff_freq;
	ohm=tanf(PI/fr);
	c=1.0f+2.0f*cosf(PI/4.0f)*ohm + ohm*ohm;
	
    filterObject->_cutoff_freq1 = cutoff_freq;
    if (filterObject->_cutoff_freq1 > 0.0f) {
		filterObject->_b01 = ohm*ohm/c;
		filterObject->_b11 = 2.0f*filterObject->_b01;
		filterObject->_b21 = filterObject->_b01;
		filterObject->_a11 = 2.0f*(ohm*ohm-1.0f)/c;
		filterObject->_a21 = (1.0f-2.0f*cosf(PI/4.0f)*ohm+ohm*ohm)/c;
	}
}

float LPF2pApply_1(LPF2pFilter_TypeDef *filterObject, float sample)
{
	
	float delay_element_0 = 0, output=0;
    if (filterObject->_cutoff_freq1 <= 0.0f) {
        // no filtering
        return sample;
    } else {
		delay_element_0 = 
			sample - 
			filterObject->_delay_element_11 * filterObject->_a11 - 
			filterObject->_delay_element_21 * filterObject->_a21;
			// do the filtering
			if (isnan(delay_element_0) || isinf(delay_element_0)) {
				// don't allow bad values to propogate via the filter
				delay_element_0 = sample;
			}
		output = 	delay_element_0 * filterObject->_b01 + 
					filterObject->_delay_element_11 * filterObject->_b11 + 
					filterObject->_delay_element_21 * filterObject->_b21;
		
		filterObject->_delay_element_21 = filterObject->_delay_element_11;
		filterObject->_delay_element_11 = delay_element_0;

		// return the value.  Should be no need to check limits
		return output;
	}
}
