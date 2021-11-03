#include <cassert>
#include <raptordsp/misc/wav.h>

#define WRITE_TAG(buffer, offset, tag) buffer[offset++] = tag[0]; buffer[offset++] = tag[1]; buffer[offset++] = tag[2]; buffer[offset++] = tag[3];
#define WRITE_INT32(buffer, offset, value) *((int*)&buffer[offset]) = value; offset += 4;
#define WRITE_INT16(buffer, offset, value) *((short*)&buffer[offset]) = value; offset += 2;

#define READ_TAG(buffer, offset, result) result[0] = buffer[offset++]; result[1] = buffer[offset++]; result[2] = buffer[offset++]; result[3] = buffer[offset++];
#define READ_INT32(buffer, offset, result) result = *((int*)&buffer[offset]); offset += 4;
#define READ_INT16(buffer, offset, result) result = *((short*)&buffer[offset]); offset += 2;

#define COMPARE_TAG(tagA, tagB) ((tagA[0] == tagB[0]) && (tagA[1] == tagB[1]) && (tagA[2] == tagB[2]) && (tagA[3] == tagB[3]))

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

bool read_wav_header(unsigned char buffer[WAV_HEADER_SIZE], wav_header_data* output) {
    //Init all values
    char tagRiff[4];
    int length;
    char tagWave[4];
    char tagFmt[4];
    int fmtLength;
    short formatTag;
    short channels;
    int sampleRate;
    int avgBytesPerSec;
    short blockAlign;
    short bitsPerSample;
    char tagData[4];
    int dataLen;

    //Read
    int offset = 0;
    READ_TAG(buffer, offset, tagRiff);
    READ_INT32(buffer, offset, length);
    READ_TAG(buffer, offset, tagWave);
    READ_TAG(buffer, offset, tagFmt);
    READ_INT32(buffer, offset, fmtLength);
    READ_INT16(buffer, offset, formatTag);
    READ_INT16(buffer, offset, channels);
    READ_INT32(buffer, offset, sampleRate);
    READ_INT32(buffer, offset, avgBytesPerSec);
    READ_INT16(buffer, offset, blockAlign);
    READ_INT16(buffer, offset, bitsPerSample);
    READ_TAG(buffer, offset, tagData);
    READ_INT32(buffer, offset, dataLen);
    assert(offset == WAV_HEADER_SIZE);

    //Validate tags
    if(!COMPARE_TAG(tagRiff, "RIFF") || !COMPARE_TAG(tagWave, "WAVE") || !COMPARE_TAG(tagFmt, "fmt "))
        return false;

    //Parse
    output->channels = channels;
    output->length = dataLen;
    output->sample_rate = sampleRate;
    output->bits_per_sample = bitsPerSample;

    return true;
}