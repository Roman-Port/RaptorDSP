#include <raptordsp/analog/rotator.h>
#include <volk/volk.h>

raptor_rotator::raptor_rotator() : raptor_rotator(0) {

}

raptor_rotator::raptor_rotator(float freqOffset) :
	sample_rate(0),
	freq_offset(freqOffset),
	phase_inc(0, 0),
	phase(1, 0)
{

}

void raptor_rotator::set_sample_rate(float sampleRate) {
	sample_rate = sampleRate;
	configure();
}

void raptor_rotator::set_freq_offset(float freqOffset) {
	freq_offset = freqOffset;
	configure();
}

void raptor_rotator::configure() {
	//Check if it's valid or else we'll divide by zero
	if (sample_rate == 0) {
		phase_inc = raptor_complex(0, 0);
		return;
	}

	//Calculate
	float angle = 2 * M_PI * freq_offset / sample_rate;
	phase_inc.real(std::cos(angle));
	phase_inc.imag(std::sin(angle));
}

void raptor_rotator::process(const raptor_complex* input, raptor_complex* output, int count) {
	volk_32fc_s32fc_x2_rotator_32fc(output, input, phase_inc, &phase, count);
}