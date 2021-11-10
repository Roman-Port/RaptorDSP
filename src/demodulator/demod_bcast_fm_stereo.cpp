#include <raptordsp/demodulator/demod_bcast_fm_stereo.h>
#include <raptordsp/filter/builder/builder_bandpass.h>
#include <cassert>
#include <complex>

demod_bcast_fm_stereo::demod_bcast_fm_stereo(size_t buffer_size, float deemphasisTime) : demod_bcast_fm(buffer_size, deemphasisTime),
    delay(),
    pll(),
    stereo_enabled(true)
{
    pilot_buffer = (raptor_complex*)malloc(sizeof(raptor_complex) * buffer_size);
    pll_buffer = (raptor_complex*)malloc(sizeof(raptor_complex) * buffer_size);
}

demod_bcast_fm_stereo::~demod_bcast_fm_stereo() {
    //Free buffers
    free(pilot_buffer);
    free(pll_buffer);
    pilot_buffer = nullptr;
    pll_buffer = nullptr;

    //Destroy delay line
    delete delay;
    delay = nullptr;
}

float demod_bcast_fm_stereo::configure(float sampleRate) {
    //Call the super configure function
    float audioSampleRate = demod_bcast_fm::configure(sampleRate);

    //Configure pilot filter
    raptor_filter_builder_bandpass pilotFilterBuilder(sampleRate, 18500, 19500);
    pilotFilterBuilder.automatic_tap_count(1000, 30);
    pilot_filter.configure(pilotFilterBuilder.build_taps_complex(), pilotFilterBuilder.ntaps, 1);

    //Make delay line
    delay = new raptor_delay_line<float>(pilotFilterBuilder.ntaps - ((pilotFilterBuilder.ntaps - 1) / 2), 0);

    //Configure PLL
    pll.configure(/*2 * M_PI * 8 / sampleRate*/ M_PI / 200,
        2 * M_PI * (19000 + 4) / sampleRate,
        2 * M_PI * (19000 - 4) / sampleRate);

    //Configure our difference audio filter
    assert(audioSampleRate == create_audio_filter(&filter_stereo, sampleRate));

    return audioSampleRate;
}

void demod_bcast_fm_stereo::baseband_demodulated(int count) {
    //Copy from the MPX buffer to the pilot buffer
    for (int i = 0; i < count; i++)
        pilot_buffer[i] = raptor_complex(mpx_buffer[i], 0);

    //Delay the MPX buffer for audio, we'll come back to it later
    delay->process(mpx_buffer, count);

    //Filter to remove pilot
    pilot_filter.process(pilot_buffer, pilot_buffer, count, 1);

    //Process PLL on pilot
    pll.process(pilot_buffer, pll_buffer, count);
}

void demod_bcast_fm_stereo::audio_filtered(float* audioBufferL, float* audioBufferR, int mpxCount, int audioLength) {
    //Demodulate L-R using the stereo pilot
    for (int i = 0; i < mpxCount; i++)
        audioBufferR[i] = mpx_buffer[i] * std::imag(pll_buffer[i] * pll_buffer[i]);

    //Filter and decimate L-R
    assert(audioLength == filter_stereo.process(audioBufferR, audioBufferR, mpxCount, 1));

    //If stereo is detected, enter recovery. Otherwise, copy L to R as usual
    if (is_stereo_detected() && stereo_enabled) {
        //Recover L and R audio channels
        float add;
        float sub;
        for (int i = 0; i < audioLength; i++)
        {
            add = audioBufferL[i]; //L+R signal
            sub = 2 * audioBufferR[i]; //L-R signal
            audioBufferL[i] = (add + sub);
            audioBufferR[i] = (add - sub);
        }
    }
    else {
        //Process as if this was mono
        demod_bcast_fm::audio_filtered(audioBufferL, audioBufferR, mpxCount, audioLength);
    }
}

bool demod_bcast_fm_stereo::is_stereo_detected() {
    return pll.get_avg_error() < 0.01f;
}