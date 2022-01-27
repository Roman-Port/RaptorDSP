#include <raptordsp/filter/fir/filter_base.h>
#include <volk/volk.h>

/*
	
	raptor_filter_fff

	INPUT  : FLOAT
	OUTPUT : FLOAT
	TAPS   : FLOAT

*/

template <>
void raptor_filter_fff::compute(float* result, const float* input, const float* taps, unsigned int num_points) {
	volk_32f_x2_dot_prod_32f(result, input, taps, num_points);
}

template <>
void raptor_filter_fff::set_taps_from_builder(raptor_filter_builder_base* builder) {
	builder->build_taps_real(taps_buffer);
}

/*

	raptor_filter_ccf

	INPUT  : COMPLEX
	OUTPUT : COMPLEX
	TAPS   : FLOAT

*/

template <>
void raptor_filter_ccf::compute(raptor_complex* result, const raptor_complex* input, const float* taps, unsigned int num_points) {
	volk_32fc_32f_dot_prod_32fc(result, input, taps, num_points);
}

template <>
void raptor_filter_ccf::set_taps_from_builder(raptor_filter_builder_base* builder) {
	builder->build_taps_real(taps_buffer);
}

/*

	raptor_filter_fcc

	INPUT  : FLOAT
	OUTPUT : COMPLEX
	TAPS   : COMPLEX

*/

template <>
void raptor_filter_fcc::compute(raptor_complex* result, const float* input, const raptor_complex* taps, unsigned int num_points) {
	volk_32fc_32f_dot_prod_32fc(result, taps, input, num_points); //we've swapped the taps and the input. apperently that's valid? https://github.com/gnuradio/gnuradio/blob/master/gr-filter/lib/fir_filter.cc#L122
}

template <>
void raptor_filter_fcc::set_taps_from_builder(raptor_filter_builder_base* builder) {
	builder->build_taps_complex(taps_buffer);
}

/*

	raptor_filter_ccc

	INPUT  : COMPLEX
	OUTPUT : COMPLEX
	TAPS   : COMPLEX

*/

template <>
void raptor_filter_ccc::compute(raptor_complex* result, const raptor_complex* input, const raptor_complex* taps, unsigned int num_points) {
	volk_32fc_x2_dot_prod_32fc(result, input, taps, num_points);
}

template <>
void raptor_filter_ccc::set_taps_from_builder(raptor_filter_builder_base* builder) {
	builder->build_taps_complex(taps_buffer);
}