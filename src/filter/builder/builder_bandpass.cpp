#include <raptordsp/filter/builder/builder_bandpass.h>
#include <raptordsp/filter/builder/builder_lowpass.h>

raptor_filter_builder_bandpass::raptor_filter_builder_bandpass(float sampleRate, float lowCutoffFreq, float highCutoffFreq) : raptor_filter_builder_base(sampleRate), lowCutoffFreq(lowCutoffFreq), highCutoffFreq(highCutoffFreq), gain(1) {

}

float raptor_filter_builder_bandpass::get_max_filter_cutoff() {
    return highCutoffFreq;
}

void raptor_filter_builder_bandpass::build_taps_real_internal(float* taps, int ntaps) {
    float* w = raptor_window_build(window_type, ntaps, param, false);

    int M = (ntaps - 1) / 2;
    double fwT0 = 2 * M_PI * lowCutoffFreq / sampleRate;
    double fwT1 = 2 * M_PI * highCutoffFreq / sampleRate;

    for (int n = -M; n <= M; n++) {
        if (n == 0)
            taps[n + M] = (fwT1 - fwT0) / M_PI * w[n + M];
        else {
            taps[n + M] = (sin(n * fwT1) - sin(n * fwT0)) / (n * M_PI) * w[n + M];
        }
    }

    free(w);

    // find the factor to normalize the gain, fmax.
    // For band-pass, gain @ center freq = 1.0

    double fmax = taps[0 + M];
    for (int n = 1; n <= M; n++)
        fmax += 2 * taps[n + M] * cos(n * (fwT0 + fwT1) * 0.5);

    gain /= fmax; // normalize

    for (int i = 0; i < ntaps; i++)
        taps[i] *= gain;
}

void raptor_filter_builder_bandpass::build_taps_complex_internal(raptor_complex* output, int ntaps) {
    //Construct base filter
    float* baseTaps = (float*)malloc(sizeof(float) * ntaps);
    {
        raptor_filter_builder_lowpass baseBuilder(sampleRate, (highCutoffFreq - lowCutoffFreq) / 2);
        baseBuilder.set_ntaps(ntaps);
        baseBuilder.window_type = window_type;
        baseBuilder.param = param;
        baseBuilder.build_taps_real(baseTaps);
    }

    //Calculate freq
    float freq = M_PI * (highCutoffFreq + lowCutoffFreq) / sampleRate;

    //Calculate initial phase
    float phase;
    if ((ntaps & 1) != 0)
        phase = -freq * (ntaps >> 1);
    else
        phase = -freq / 2 * ((1 + 2 * ntaps) >> 1);

    //Generate
    float* outputFloat = (float*)output;
    for (int i = 0; i < ntaps; i++)
    {
#ifdef sincosf
        sincosf(phase, &outputFloat[0], &outputFloat[1]);
#else
        outputFloat[0] = std::sin(phase);
        outputFloat[1] = std::cos(phase);
#endif
        outputFloat[0] *= baseTaps[i];
        outputFloat[1] *= baseTaps[i];
        outputFloat += 2;
        phase += freq;
    }

    //Clean up
    free(baseTaps);
}