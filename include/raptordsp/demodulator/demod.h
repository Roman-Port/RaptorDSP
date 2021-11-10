#pragma once

#include <raptordsp/defines.h>

class demod {

public:
	virtual float configure(float sampleRate) = 0;
	virtual int process(raptor_complex* iq, int count, float* audioL, float* audioR) = 0;

};