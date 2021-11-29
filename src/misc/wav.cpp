#include <cassert>
#include <stdint.h>
#include <cstring>
#include <raptordsp/misc/wav.h>

#define WRITE_TAG(buffer, offset, tag) buffer[offset++] = tag[0]; buffer[offset++] = tag[1]; buffer[offset++] = tag[2]; buffer[offset++] = tag[3];
#define WRITE_INT32(buffer, offset, value) *((int*)&buffer[offset]) = value; offset += 4;
#define WRITE_INT16(buffer, offset, value) *((short*)&buffer[offset]) = value; offset += 2;

void create_wav_header(unsigned char buffer[WAV_HEADER_SIZE], int length, short channels, int sampleRate, int bitsPerSample) {

    //Calculate
    short blockAlign = (short)(channels * (bitsPerSample / 8));
    int avgBytesPerSec = sampleRate * (int)blockAlign;

    //Write
    int offset = 0;
    WRITE_TAG(buffer, offset, "RIFF");
    WRITE_INT32(buffer, offset, length);
    WRITE_TAG(buffer, offset, "WAVE");
    WRITE_TAG(buffer, offset, "fmt ");
    WRITE_INT32(buffer, offset, 16);
    WRITE_INT16(buffer, offset, 1); //Format tag
    WRITE_INT16(buffer, offset, channels);
    WRITE_INT32(buffer, offset, sampleRate);
    WRITE_INT32(buffer, offset, avgBytesPerSec);
    WRITE_INT16(buffer, offset, blockAlign);
    WRITE_INT16(buffer, offset, bitsPerSample);
    WRITE_TAG(buffer, offset, "data");
    WRITE_INT32(buffer, offset, length); //Length
    assert(offset == WAV_HEADER_SIZE);
}

template <class T>
T _wav_read(uint8_t** buffer, size_t* remaining) {
    //Make sure we have enough space remaining
    assert(*remaining >= sizeof(T));

    //Read
    T value;
    memcpy(&value, *buffer, sizeof(T));

    //Update state
    (*buffer) += sizeof(T);
    (*remaining) -= sizeof(T);

    return value;
}

bool read_wav_header(uint8_t buffer[WAV_HEADER_SIZE], wav_header_data* output) {
    //Read all
    size_t remaining = WAV_HEADER_SIZE;
    uint32_t tagRiff            = _wav_read<uint32_t>(&buffer, &remaining);
    int32_t length              = _wav_read<int32_t>(&buffer, &remaining);
    uint32_t tagWave            = _wav_read<uint32_t>(&buffer, &remaining);
    uint32_t tagFmt             = _wav_read<uint32_t>(&buffer, &remaining);
    int32_t fmtLength           = _wav_read<int32_t>(&buffer, &remaining);
    int16_t formatTag           = _wav_read<int16_t>(&buffer, &remaining);
    int16_t channels            = _wav_read<int16_t>(&buffer, &remaining);
    int32_t sampleRate          = _wav_read<int32_t>(&buffer, &remaining);
    int32_t avgBytesPerSec      = _wav_read<int32_t>(&buffer, &remaining);
    int16_t blockAlign          = _wav_read<int16_t>(&buffer, &remaining);
    int16_t bitsPerSample       = _wav_read<int16_t>(&buffer, &remaining);
    uint32_t tagData            = _wav_read<uint32_t>(&buffer, &remaining);
    int32_t dataLen             = _wav_read<int32_t>(&buffer, &remaining);
    assert(remaining == 0);

    //Validate tags
    if(tagRiff != 1179011410 || tagWave != 1163280727 || tagFmt != 544501094)
        return false;

    //Parse
    output->channels = channels;
    output->length = dataLen;
    output->sample_rate = sampleRate;
    output->bits_per_sample = bitsPerSample;
    
    return true;
}