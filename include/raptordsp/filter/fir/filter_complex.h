#ifndef RAPTORDSP_FILTER_FIR_COMPLEX
#define RAPTORDSP_FILTER_FIR_COMPLEX

#include <raptordsp/defines.h>

class raptor_filter_complex {

public:
    raptor_filter_complex();
    void configure(raptor_complex* coeffs, int coeffsCount, int decimation);
    int process(raptor_complex* input, raptor_complex* output, int count, int channels);

private:
    raptor_complex* coeffsBufferPtr;
    raptor_complex* insampBufferPtr;
    raptor_complex* insampBufferPtrOffset;
    raptor_complex* tempBufferPtr; //used to keep VOLK aligned
    int taps;
    int decimation;
    int decimationIndex;
    int offset;

};

#endif
