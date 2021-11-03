#include <raptordsp/filter/fir/filter_complex.h>
#include <volk/volk.h>

raptor_filter_complex::raptor_filter_complex() : coeffsBufferPtr(0), insampBufferPtr(0), insampBufferPtrOffset(0), tempBufferPtr(0), taps(0), decimation(0), decimationIndex(0), offset(0) {

}

void raptor_filter_complex::configure(raptor_complex* coeffs, int coeffsCount, int decimation) {
    //Create aligned buffers
    size_t alignment = volk_get_alignment();
    coeffsBufferPtr = (raptor_complex*)volk_malloc(coeffsCount * sizeof(raptor_complex), alignment);
    insampBufferPtr = (raptor_complex*)volk_malloc((size_t)coeffsCount * 2 * sizeof(raptor_complex), alignment);
    tempBufferPtr = (raptor_complex*)volk_malloc(1 * sizeof(raptor_complex), alignment);

    //Apply settings
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

int raptor_filter_complex::process(raptor_complex* input, raptor_complex* output, int count, int channels) {
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
                volk_32fc_x2_dot_prod_32fc(tempBufferPtr, &insampBufferPtr[offset], coeffsBufferPtr, taps);
                output[read++ * channels] = tempBufferPtr[0];
            }
            this->decimationIndex %= this->decimation;
        }
        return read;
}