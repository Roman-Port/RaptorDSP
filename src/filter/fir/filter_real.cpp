#include <raptordsp/filter/fir/filter_real.h>
#include <volk/volk.h>

raptor_filter_real::raptor_filter_real() {

}

void raptor_filter_real::configure(float* coeffs, int coeffsCount, int decimation) {
    //Create aligned buffers
    size_t alignment = volk_get_alignment();
    coeffsBufferPtr = (float*)volk_malloc(coeffsCount * sizeof(float), alignment);
    insampBufferPtr = (float*)volk_malloc((size_t)coeffsCount * 2 * sizeof(float), alignment);
    tempBufferPtr = (float*)volk_malloc(1 * sizeof(float), alignment);

    //Apply setings
    insampBufferPtrOffset = &insampBufferPtr[coeffsCount];
    taps = coeffsCount;
    this->decimation = decimation;
    decimationIndex = 0;
    offset = 0;

    //Copy taps
    for (int i = 0; i < coeffsCount; i++)
        coeffsBufferPtr[i] = coeffs[i];

    //Clear allocated arrays
    tempBufferPtr[0] = 0;
    for (int i = 0; i < coeffsCount * 2; i++)
        insampBufferPtr[i] = 0;
}

int raptor_filter_real::process(float* input, float* output, int count, int channels) {
    int read = 0;
    for (int i = 0; i < count; i++)
    {
        //Write to both the real position as well as an offset value
        this->insampBufferPtr[this->offset] = *input;
        this->insampBufferPtrOffset[this->offset++] = *input;
        input += channels;
        this->offset %= this->taps;

        //Process (if needed)
        if (this->decimationIndex++ == 0)
        {
            volk_32f_x2_dot_prod_32f(tempBufferPtr, &insampBufferPtr[offset], coeffsBufferPtr, taps);
            output[read++ * channels] = tempBufferPtr[0];
        }
        this->decimationIndex %= this->decimation;
    }
    return read;
}