#include <raptordsp/filter/builder/builder.h>
#include <cassert>

raptor_filter_builder_base::raptor_filter_builder_base(float sampleRate) : sampleRate(sampleRate), ntaps(0), window_type(win_type::WIN_BLACKMAN_HARRIS), param(0), transitionWidth(0) {

}

int raptor_filter_builder_base::get_ntaps() {
    return ntaps;
}

void raptor_filter_builder_base::automatic_tap_count(float transitionWidth, float attenuation) {
    assert(transitionWidth > 0);
    this->transitionWidth = transitionWidth;
    int count = (int)(attenuation / (22 * (transitionWidth / sampleRate)));
    if ((count & 1) == 0) //If this is odd, make it even
        count++;
    set_ntaps(count);
}

void raptor_filter_builder_base::set_ntaps(int taps) {
    assert(taps > 0);
    this->ntaps = taps;
}

int raptor_filter_builder_base::calculate_decimation(float *actualOutputSampleRate) {
    //Calculate signal bandwidth
    float bandwidth = get_max_filter_cutoff() + transitionWidth;

    //Calculate the rate by finding the LOWEST we can go without it becoming a rate lower than the desired rate
    int decimationRate = 1;
    while (sampleRate / (decimationRate + 1) >= (bandwidth * 2)) //Multiply the bandwidth so we can run this without any aliasing
    {
        decimationRate++;
    }

    //Determine the actual output sample rate
    *actualOutputSampleRate = sampleRate / decimationRate;

    return decimationRate;
}

void raptor_filter_builder_base::build_taps_real_internal(float* converted, int ntaps) {
    throw std::runtime_error("Real taps are not supported by this builder.");
}

void raptor_filter_builder_base::build_taps_complex_internal(raptor_complex* converted, int ntaps) {
    throw std::runtime_error("Complex taps are not supported by this builder.");
}

raptor_filter_taps<float> raptor_filter_builder_base::build_taps_real() {
    float* buffer = (float*)malloc(sizeof(float) * ntaps);
    build_taps_real_internal(buffer, ntaps);
    return raptor_filter_taps<float>(buffer, ntaps);
}

raptor_filter_taps<raptor_complex> raptor_filter_builder_base::build_taps_complex() {
    raptor_complex* buffer = (raptor_complex*)malloc(sizeof(raptor_complex) * ntaps);
    build_taps_complex_internal(buffer, ntaps);
    return raptor_filter_taps<raptor_complex>(buffer, ntaps);
}