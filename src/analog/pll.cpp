#include <raptordsp/analog/pll.h>
#include <raptordsp/fast_atan2.h>

void raptor_pll::configure(float loop_bw, float max_freq, float min_freq) {
    //Set defaults
    d_phase = 0;
    d_freq = 0;
    d_max_freq = max_freq;
    d_min_freq = min_freq;

    // Set the damping factor for a critically damped system
    d_damping = sqrtf(2.0f) / 2.0f;

    //Set gains
    d_loop_bw = loop_bw;
    float denom = (1.0f + 2.0f * d_damping * d_loop_bw + d_loop_bw * d_loop_bw);
    d_alpha = (4 * d_damping * d_loop_bw) / denom;
    d_beta = (4 * d_loop_bw * d_loop_bw) / denom;

    //Reset
    d_avg_error = 0;
    d_alpha_inverse = 1 - d_alpha;
}

void raptor_pll::process(raptor_complex* iptr, raptor_complex* optr, int count) {
    float error;
    float t_imag, t_real;
    int size = count;

    while (size-- > 0) {
        sincosf(d_phase, &t_imag, &t_real);
        *optr++ = raptor_complex(t_real, t_imag);

        error = phase_detector(*iptr++, d_phase);

        advance_loop(error);
        phase_wrap();
        frequency_limit();
    }
}

float raptor_pll::phase_detector(raptor_complex sample, float ref_phase)
{
    float sample_phase = fast_atan2f(sample.imag(), sample.real());
    return mod_2pi(sample_phase - ref_phase);
}

float raptor_pll::mod_2pi(float in)
{
    if (in > M_PI) {
        return in - static_cast<float>(2.0 * M_PI);
    }
    if (in < -M_PI) {
        return in + static_cast<float>(2.0 * M_PI);
    }
    return in;
}

void raptor_pll::frequency_limit()
{
    if (d_freq > d_max_freq)
        d_freq = d_max_freq;
    else if (d_freq < d_min_freq)
        d_freq = d_min_freq;
}

void raptor_pll::phase_wrap()
{
    while (d_phase > (2 * M_PI))
        d_phase -= 2 * M_PI;
    while (d_phase < (-2 * M_PI))
        d_phase += 2 * M_PI;
}

void raptor_pll::advance_loop(float error)
{
    d_avg_error = d_alpha_inverse * d_avg_error + d_alpha * error * error;
    d_freq = d_freq + d_beta * error;
    d_phase = d_phase + d_freq + d_alpha * error;
}