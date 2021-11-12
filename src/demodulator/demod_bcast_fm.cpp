#include <raptordsp/demodulator/demod_bcast_fm.h>
#include <volk/volk.h>
#include <raptordsp/fast_atan2.h>
#include <raptordsp/filter/builder/builder_lowpass.h>

demod_bcast_fm::demod_bcast_fm(size_t bufferSize, float deemphasisTime) :
    deemphasis_time(deemphasisTime),
    deemphasis_alpha(0),
    deemphasis_state_l(0),
    deemphasis_state_r(0),
    gain(0)
{
    mpx_buffer = (float*)malloc(sizeof(float) * bufferSize);
}

demod_bcast_fm::~demod_bcast_fm() {
    //Free buffers
    free(mpx_buffer);
    mpx_buffer = nullptr;
}

float demod_bcast_fm::configure(float sampleRate) {
    //Set up FM baseband demodulation
    gain = sampleRate / (2 * M_PI * 75000);
    last_sample = 0;

    //Create audio filter
    float audioSampleRate = create_audio_filter(&filter_mono, sampleRate);

    //Configure/reset deemphasis
    deemphasis_alpha = 1.0f - exp(-1.0f / (audioSampleRate * (deemphasis_time * 1e-6f)));
    deemphasis_state_l = 0;
    deemphasis_state_r = 0;

    return audioSampleRate;
}

int demod_bcast_fm::process(raptor_complex* iqBuffer, int count, float* audioBufferL, float* audioBufferR) {
    //Demodulate baseband signal
    lv_32fc_t temp;
    for (int i = 0; i < count; i++) {
        //Apply conjugate
        temp = iqBuffer[i] * std::conj(last_sample);

        //Estimate angle
        mpx_buffer[i] = fast_atan2f(imag(temp), real(temp)) * gain;

        //Set state
        last_sample = iqBuffer[i];
    }

    //Call virtual functions
    baseband_demodulated(count);

    //Decimate and filter L + R
    int audioLength = filter_mono.process(mpx_buffer, audioBufferL, count);

    //Call virtual function
    audio_filtered(audioBufferL, audioBufferR, count, audioLength);

    //Process deemphasis
    process_deemphasis(&deemphasis_state_l, audioBufferL, audioLength);
    process_deemphasis(&deemphasis_state_r, audioBufferR, audioLength);

    return audioLength;
}

void demod_bcast_fm::process_deemphasis(float* state, float* audio, int count) {
    for (int i = 0; i < count; i++)
    {
        *state += deemphasis_alpha * (audio[i] - *state);
        audio[i] = *state;
    }
}

float demod_bcast_fm::create_audio_filter(raptor_filter_real* filter, float sampleRate) {
    raptor_filter_builder_lowpass audioFilterBuilder(sampleRate, 15000);
    audioFilterBuilder.automatic_tap_count(1000, 50);
    filter->configure(audioFilterBuilder.build_taps_real(), audioFilterBuilder.calculate_decimation(&sampleRate));
    return sampleRate;
}

void demod_bcast_fm::audio_filtered(float* audioL, float* audioR, int mpxCount, int audioCount) {
    //Just copy from the left channel to the right channel
    memcpy(audioR, audioL, audioCount * sizeof(float));
}

void demod_bcast_fm::baseband_demodulated(int count) {

}