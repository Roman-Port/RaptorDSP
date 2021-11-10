#include <raptordsp/filter/builder/builder.h>
#include <cassert>

raptor_filter_builder_base::raptor_filter_builder_base(float sampleRate) : sampleRate(sampleRate), ntaps(0), window_type(win_type::WIN_BLACKMAN_HARRIS), param(0), transitionWidth(0) {

}

void raptor_filter_builder_base::automatic_tap_count(float transitionWidth, float attenuation) {
    assert(transitionWidth > 0);
    this->transitionWidth = transitionWidth;
    int count = (int)(attenuation / (22 * (transitionWidth / sampleRate)));
    if ((count & 1) == 0) //If this is odd, make it even
        count++;
    ntaps = count;
}

void raptor_filter_builder_base::manual_tap_count(int taps) {
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

raptor_complex* raptor_filter_builder_base::build_taps_complex() {
    //Build real taps
    float* real = build_taps_real();

    //Allocate complex taps instead
    float* converted = (float*)malloc(sizeof(raptor_complex) * ntaps);

    //Convert
    for (int i = 0; i < ntaps; i++) {
        converted[(i * 2) + 0] = real[i];
        converted[(i * 2) + 1] = real[i]; //TODO: Should this be real?
    }

    //Clean up
    free(real);

    return (raptor_complex*)converted;
}