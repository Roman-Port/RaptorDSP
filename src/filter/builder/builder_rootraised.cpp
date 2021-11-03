#include <raptordsp/filter/builder/builder_rootraised.h>

raptor_filter_builder_root_raised_cosine::raptor_filter_builder_root_raised_cosine(float sampleRate, float symbolRate, float alpha) : raptor_filter_builder_base(sampleRate), symbolRate(symbolRate), alpha(alpha), gain(1) {

}

float* raptor_filter_builder_root_raised_cosine::build_taps_real() {
    ntaps |= 1; // ensure that ntaps is odd

    double spb = sampleRate / symbolRate; // samples per bit/symbol
    float* taps = (float*)malloc(ntaps);
    double scale = 0;
    for (int i = 0; i < ntaps; i++) {
        double x1, x2, x3, num, den;
        double xindx = i - ntaps / 2;
        x1 = M_PI * xindx / spb;
        x2 = 4 * alpha * xindx / spb;
        x3 = x2 * x2 - 1;

        if (fabs(x3) >= 0.000001) { // Avoid Rounding errors...
            if (i != ntaps / 2)
                num = cos((1 + alpha) * x1) +
                      sin((1 - alpha) * x1) / (4 * alpha * xindx / spb);
            else
                num = cos((1 + alpha) * x1) + (1 - alpha) * M_PI / (4 * alpha);
            den = x3 * M_PI;
        } else {
            if (alpha == 1) {
                taps[i] = -1;
                scale += taps[i];
                continue;
            }
            x3 = (1 - alpha) * x1;
            x2 = (1 + alpha) * x1;
            num = (sin(x2) * (1 + alpha) * M_PI -
                   cos(x3) * ((1 - alpha) * M_PI * spb) / (4 * alpha * xindx) +
                   sin(x3) * spb * spb / (4 * alpha * xindx * xindx));
            den = -32 * M_PI * alpha * alpha * xindx / spb;
        }
        taps[i] = 4 * alpha * num / den;
        scale += taps[i];
    }

    for (int i = 0; i < ntaps; i++)
        taps[i] = taps[i] * gain / scale;

    return taps;
}

float raptor_filter_builder_root_raised_cosine::get_max_filter_cutoff() {
    return 0;
}