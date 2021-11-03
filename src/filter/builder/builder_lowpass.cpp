#include <raptordsp/filter/builder/builder_lowpass.h>

raptor_filter_builder_lowpass::raptor_filter_builder_lowpass(float sampleRate, float cutoffFreq) : raptor_filter_builder_base(sampleRate), cutoffFreq(cutoffFreq), gain(1) {

}

float* raptor_filter_builder_lowpass::build_taps_real() {
    // construct the truncated ideal impulse response
    // [sin(x)/x for the low pass case]

    float* taps = (float*)malloc(sizeof(float) * ntaps);
    float* w = raptor_window_build(window_type, ntaps, param, false);

    int M = (ntaps - 1) / 2;
    double fwT0 = 2 * M_PI * cutoffFreq / sampleRate;

    for (int n = -M; n <= M; n++) {
        if (n == 0)
            taps[n + M] = fwT0 / M_PI * w[n + M];
        else {
            // a little algebra gets this into the more familiar sin(x)/x form
            taps[n + M] = sin(n * fwT0) / (n * M_PI) * w[n + M];
        }
    }

    // find the factor to normalize the gain, fmax.
    // For low-pass, gain @ zero freq = 1.0

    double fmax = taps[0 + M];
    for (int n = 1; n <= M; n++)
        fmax += 2 * taps[n + M];

    gain /= fmax; // normalize

    for (int i = 0; i < ntaps; i++)
        taps[i] *= gain;

    return taps;
}

float raptor_filter_builder_lowpass::get_max_filter_cutoff() {
    return cutoffFreq;
}