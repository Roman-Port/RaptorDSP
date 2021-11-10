#include <raptordsp/filter/window.h>
#include <raptordsp/defines.h>
#include <stdexcept>
#include <cmath>

double Izero(double x)
{
    double sum, u, halfx, temp;
    int n;

    sum = u = n = 1;
    halfx = x / 2.0;
    do {
        temp = halfx / (double)n;
        n += 1;
        temp *= temp;
        u *= temp;
        sum += u;
    } while (u >= 1E-21 * sum);
    return (sum);
}

double midn(int ntaps) { return ntaps / 2.0; }

double midm1(int ntaps) { return (ntaps - 1.0) / 2.0; }

double midp1(int ntaps) { return (ntaps + 1.0) / 2.0; }

double freq(int ntaps) { return 2.0 * M_PI / ntaps; }

double rate(int ntaps) { return 1.0 / (ntaps >> 1); }

double max_attenuation(win_type type, double param)
{
    switch (type) {
        case (WIN_HAMMING):
            return 53;
        case (WIN_HANN):
            return 44;
        case (WIN_BLACKMAN):
            return 74;
        case (WIN_RECTANGULAR):
            return 21;
        case (WIN_KAISER):
            // linear approximation
            return (param / 0.1102 + 8.7);
        case (WIN_BLACKMAN_hARRIS):
            return 92;
        case (WIN_BARTLETT):
            return 27;
        case (WIN_FLATTOP):
            return 93;
        case WIN_NUTTALL:
            return 114;
        case WIN_NUTTALL_CFD:
            return 112;
        case WIN_WELCH:
            return 31;
        case WIN_PARZEN:
            return 56;
        case WIN_EXPONENTIAL:
            // varies slightly depending on the decay factor, but this is a safe return value
            return 26;
        case WIN_RIEMANN:
            return 39;
        case WIN_GAUSSIAN:
            // value not meaningful for gaussian windows, but return something reasonable
            return 100;
        case WIN_TUKEY:
            // low end is a rectangular window, attenuation exponentially approaches Hann
            // piecewise linear estimate, determined empirically via curve fitting, median
            // error is less than 0.5dB and maximum error is 2.5dB; the returned value will
            // never be less than expected attenuation to ensure that window designed filters
            // are never below expected quality.
            if (param > 0.9)
                return ((param - 0.9) * 135 + 30.5);
            else if (param > 0.7)
                return ((param - 0.6) * 20 + 24);
            return (param * 5 + 21);
        default:
            throw std::out_of_range("max_attenuation: unknown window type provided.");
    }
}

float* coswindow(int ntaps, float c0, float c1, float c2)
{
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps; n++)
        taps[n] = c0 - c1 * cosf((2.0f * M_PI * n) / M) +
                  c2 * cosf((4.0f * M_PI * n) / M);
    return taps;
}

float* coswindow(int ntaps, float c0, float c1, float c2, float c3)
{
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps; n++)
        taps[n] = c0 - c1 * cosf((2.0f * M_PI * n) / M) +
                  c2 * cosf((4.0f * M_PI * n) / M) -
                  c3 * cosf((6.0f * M_PI * n) / M);
    return taps;
}

float* coswindow(int ntaps, float c0, float c1, float c2, float c3, float c4)
{
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps; n++)
        taps[n] = c0 - c1 * cosf((2.0f * M_PI * n) / M) +
                  c2 * cosf((4.0f * M_PI * n) / M) -
                  c3 * cosf((6.0f * M_PI * n) / M) +
                  c4 * cosf((8.0f * M_PI * n) / M);
    return taps;
}

float* rectangular(int ntaps)
{
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    for (int n = 0; n < ntaps; n++)
        taps[n] = 1;
    return taps;
}

float* hamming(int ntaps)
{
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps; n++)
        taps[n] = 0.54 - 0.46 * cos((2 * M_PI * n) / M);
    return taps;
}

float* hann(int ntaps)
{
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps; n++)
        taps[n] = 0.5 - 0.5 * cos((2 * M_PI * n) / M);
    return taps;
}

float* hanning(int ntaps) { return hann(ntaps); }

float* blackman(int ntaps)
{
    return coswindow(ntaps, 0.42, 0.5, 0.08);
}

float* blackman2(int ntaps)
{
    return coswindow(ntaps, 0.34401, 0.49755, 0.15844);
}

float* blackman3(int ntaps)
{
    return coswindow(ntaps, 0.21747, 0.45325, 0.28256, 0.04672);
}

float* blackman4(int ntaps)
{
    return coswindow(ntaps, 0.084037, 0.29145, 0.375696, 0.20762, 0.041194);
}

float* blackman_harris(int ntaps, int atten = 92)
{
    switch (atten) {
        case (61):
            return coswindow(ntaps, 0.42323, 0.49755, 0.07922);
        case (67):
            return coswindow(ntaps, 0.44959, 0.49364, 0.05677);
        case (74):
            return coswindow(ntaps, 0.40271, 0.49703, 0.09392, 0.00183);
        case (92):
            return coswindow(ntaps, 0.35875, 0.48829, 0.14128, 0.01168);
        default:
            throw std::out_of_range("blackman_harris: unknown attenuation value "
                                    "(must be 61, 67, 74, or 92)");
    }
}

float* blackmanharris(int ntaps, int atten)
{
    return blackman_harris(ntaps, atten);
}

float* nuttall(int ntaps)
{
    return coswindow(ntaps, 0.3635819, 0.4891775, 0.1365995, 0.0106411);
}

float* blackman_nuttall(int ntaps) { return nuttall(ntaps); }
float* nuttall_cfd(int ntaps)
{
    return coswindow(ntaps, 0.355768, 0.487396, 0.144232, 0.012604);
}

float* flattop(int ntaps)
{
    double scale = 4.63867;
    return coswindow(
            ntaps, 1.0 / scale, 1.93 / scale, 1.29 / scale, 0.388 / scale, 0.028 / scale);
}

float* kaiser(int ntaps, double beta)
{
    if (beta < 0)
        throw std::out_of_range("kaiser: beta must be >= 0");

    float* taps = (float*)malloc(sizeof(float) * ntaps);

    double IBeta = 1.0 / Izero(beta);
    double inm1 = 1.0 / ((double)(ntaps - 1));
    double temp;

    /* extracting first and last element out of the loop, since
       sqrt(1.0-temp*temp) might trigger unexpected floating point behaviour
       if |temp| = 1.0+epsilon, which can happen for i==0 and
       1/i==1/(ntaps-1)==inm1 ; compare
       https://github.com/gnuradio/gnuradio/issues/1348 .
       In any case, the 0. Bessel function of first kind is 1 at point 0.
     */
    taps[0] = IBeta;
    for (int i = 1; i < ntaps - 1; i++) {
        temp = 2 * i * inm1 - 1;
        taps[i] = Izero(beta * sqrt(1.0 - temp * temp)) * IBeta;
    }
    taps[ntaps - 1] = IBeta;
    return taps;
}

float* bartlett(int ntaps)
{
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    float M = static_cast<float>(ntaps - 1);

    for (int n = 0; n < ntaps / 2; n++)
        taps[n] = 2 * n / M;
    for (int n = ntaps / 2; n < ntaps; n++)
        taps[n] = 2 - 2 * n / M;

    return taps;
}

float* welch(int ntaps)
{
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    double m1 = midm1(ntaps);
    double p1 = midp1(ntaps);
    for (int i = 0; i < midn(ntaps) + 1; i++) {
        taps[i] = 1.0 - pow((i - m1) / p1, 2);
        taps[ntaps - i - 1] = taps[i];
    }
    return taps;
}

float* parzen(int ntaps)
{
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    double m1 = midm1(ntaps);
    double m = midn(ntaps);
    int i;
    for (i = ntaps / 4; i < 3 * ntaps / 4; i++) {
        taps[i] = 1.0 - 6.0 * pow((i - m1) / m, 2.0) * (1.0 - fabs(i - m1) / m);
    }
    for (; i < ntaps; i++) {
        taps[i] = 2.0 * pow(1.0 - fabs(i - m1) / m, 3.0);
        taps[ntaps - i - 1] = taps[i];
    }
    return taps;
}

float* exponential(int ntaps, double d)
{
    if (d < 0)
        throw std::out_of_range("exponential: d must be >= 0");

    double m1 = midm1(ntaps);
    double p = 1.0 / (8.69 * ntaps / (2.0 * d));
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    for (int i = 0; i < midn(ntaps) + 1; i++) {
        taps[i] = exp(-fabs(i - m1) * p);
        taps[ntaps - i - 1] = taps[i];
    }
    return taps;
}

float* riemann(int ntaps)
{
    double cx;
    double sr1 = freq(ntaps);
    double mid = midn(ntaps);
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    for (int i = 0; i < mid; i++) {
        if (i == midn(ntaps)) {
            taps[i] = 1.0;
            taps[ntaps - i - 1] = 1.0;
        }
        else {
            cx = sr1 * (i - mid);
            taps[i] = sin(cx) / cx;
            taps[ntaps - i - 1] = sin(cx) / cx;
        }
    }
    return taps;
}

float* tukey(int ntaps, float alpha)
{
    if ((alpha < 0) || (alpha > 1))
        throw std::out_of_range("tukey: alpha must be between 0 and 1");

    float N = static_cast<float>(ntaps - 1);

    float aN = alpha * N;
    float p1 = aN / 2.0;
    float mid = midn(ntaps);
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    for (int i = 0; i < mid; i++) {
        if (abs(i) < p1) {
            taps[i] = 0.5 * (1.0 - cos((2 * M_PI * i) / (aN)));
            taps[ntaps - 1 - i] = taps[i];
        }
        else {
            taps[i] = 1.0;
            taps[ntaps - i - 1] = 1.0;
        }
    }
    return taps;
}

float* gaussian(int ntaps, float sigma)
{
    if (sigma <= 0)
        throw std::out_of_range("gaussian: sigma must be > 0");

    float a = 2 * sigma * sigma;
    double m1 = midm1(ntaps);
    float* taps = (float*)malloc(sizeof(float) * ntaps);
    for (int i = 0; i < midn(ntaps); i++) {
        float N = (i - m1);
        taps[i] = exp(-(N * N / a));
        taps[ntaps - 1 - i] = taps[i];
    }
    return taps;
}

float* raptor_window_build(win_type type, int ntaps, double param, const bool normalize)
{
    // If we want a normalized window, we get a non-normalized one first, then
    // normalize it here:
    if (normalize) {
        auto win = raptor_window_build(type, ntaps, param, false);

        // sum(win**2) / len(win)
        double pwr_acc = 0;
        for (int i = 0; i < ntaps; i++)
            pwr_acc += win[i] * win[i];
        pwr_acc /= ntaps;

        //Transform all
        const float norm_fac = static_cast<float>(std::sqrt(pwr_acc));
        for (int i = 0; i < ntaps; i++)
            win[i] /= norm_fac;
        return win;
    }

    // Create non-normalized window:
    switch (type) {
        case WIN_RECTANGULAR:
            return rectangular(ntaps);
        case WIN_HAMMING:
            return hamming(ntaps);
        case WIN_HANN:
            return hann(ntaps);
        case WIN_BLACKMAN:
            return blackman(ntaps);
        case WIN_BLACKMAN_hARRIS:
            return blackman_harris(ntaps);
        case WIN_KAISER:
            return kaiser(ntaps, param);
        case WIN_BARTLETT:
            return bartlett(ntaps);
        case WIN_FLATTOP:
            return flattop(ntaps);
        case WIN_NUTTALL:
            return nuttall(ntaps);
        case WIN_NUTTALL_CFD:
            return nuttall_cfd(ntaps);
        case WIN_WELCH:
            return welch(ntaps);
        case WIN_PARZEN:
            return parzen(ntaps);
        case WIN_EXPONENTIAL:
            return exponential(ntaps, param);
        case WIN_RIEMANN:
            return riemann(ntaps);
        case WIN_GAUSSIAN:
            return gaussian(ntaps, param);
        case WIN_TUKEY:
            return tukey(ntaps, param);
        default:
            throw std::out_of_range("build: type out of range");
    }
}