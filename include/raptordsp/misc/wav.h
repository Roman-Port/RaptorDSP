#ifndef ANDROIDSDRTUNER_WAV_H
#define ANDROIDSDRTUNER_WAV_H

#define WAV_HEADER_SIZE 44

struct wav_header_data {
    int length;
    short channels;
    int sample_rate;
    int bits_per_sample;
};

void create_wav_header(unsigned char buffer[WAV_HEADER_SIZE], int length, short channels, int sampleRate, int bitsPerSample);
bool read_wav_header(unsigned char buffer[WAV_HEADER_SIZE], wav_header_data* output);

#endif //ANDROIDSDRTUNER_WAV_H
