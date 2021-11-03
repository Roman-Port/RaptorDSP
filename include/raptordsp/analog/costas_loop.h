#ifndef RAPTORDSP_ANALOG_COSTASLOOP
#define RAPTORDSP_ANALOG_COSTASLOOP

#include <raptordsp/defines.h>

class raptor_costas_loop {

public:
    void configure(float loop_bw, int order, bool use_snr);
    void process(raptor_complex* iptr, raptor_complex* optr, int count);

private:
    float d_phase, d_freq;
    float d_max_freq, d_min_freq;
    float d_damping, d_loop_bw;
    float d_alpha, d_beta;

    float d_error;
    float d_noise;
    bool d_use_snr;
    int d_order;

    void update_gains();

    void advance_loop(float error)
    {
        d_freq = d_freq + d_beta * error;
        d_phase = d_phase + d_freq + d_alpha * error;
    }

    void phase_wrap()
    {
        while (d_phase > (2 * M_PI))
            d_phase -= 2 * M_PI;
        while (d_phase < (-2 * M_PI))
            d_phase += 2 * M_PI;
    }

    void frequency_limit()
    {
        if (d_freq > d_max_freq)
            d_freq = d_max_freq;
        else if (d_freq < d_min_freq)
            d_freq = d_min_freq;
    }

    static float tanhf_lut(float x);

    float phase_detector_8(raptor_complex sample) const // for 8PSK
    {
        const float K = (sqrtf(2.0) - 1);
        if (fabsf(sample.real()) >= fabsf(sample.imag())) {
            return ((sample.real() > 0.0f ? 1.0f : -1.0f) * sample.imag() -
                    (sample.imag() > 0.0f ? 1.0f : -1.0f) * sample.real() * K);
        } else {
            return ((sample.real() > 0.0f ? 1.0f : -1.0f) * sample.imag() * K -
                    (sample.imag() > 0.0f ? 1.0f : -1.0f) * sample.real());
        }
    };

    float phase_detector_2(raptor_complex sample) const // for BPSK
    {
        return (sample.real() * sample.imag());
    }

    float phase_detector_4(raptor_complex sample) const // for QPSK
    {
        return ((sample.real() > 0.0f ? 1.0f : -1.0f) * sample.imag() -
                (sample.imag() > 0.0f ? 1.0f : -1.0f) * sample.real());
    };

    float phase_detector_snr_8(raptor_complex sample) const // for 8PSK
    {
        const float K = (sqrtf(2.0) - 1.0);
        const float snr = std::norm(sample) / d_noise;
        if (fabsf(sample.real()) >= fabsf(sample.imag())) {
            return ((tanhf_lut(snr * sample.real()) * sample.imag()) -
                    (tanhf_lut(snr * sample.imag()) * sample.real() * K));
        } else {
            return ((tanhf_lut(snr * sample.real()) * sample.imag() * K) -
                    (tanhf_lut(snr * sample.imag()) * sample.real()));
        }
    };

    float phase_detector_snr_4(raptor_complex sample) const // for QPSK
    {
        const float snr = std::norm(sample) / d_noise;
        return ((tanhf_lut(snr * sample.real()) * sample.imag()) -
                (tanhf_lut(snr * sample.imag()) * sample.real()));
    };

    float phase_detector_snr_2(raptor_complex sample) const // for BPSK
    {
        const float snr = std::norm(sample) / d_noise;
        return tanhf_lut(snr * sample.real()) * sample.imag();
    };

};

#endif