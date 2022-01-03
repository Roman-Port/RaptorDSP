#pragma once

#include <raptordsp/defines.h>

class raptor_rotator {

public:
	raptor_rotator();
	raptor_rotator(float freqOffset);
	void set_sample_rate(float sampleRate);
	void set_freq_offset(float freqOffset);
	void process(const raptor_complex* input, raptor_complex* output, int count);

private:
	void configure();

	float freq_offset;
	float sample_rate;

	raptor_complex phase_inc;
	raptor_complex phase;

};