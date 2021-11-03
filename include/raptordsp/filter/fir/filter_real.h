#ifndef RAPTORDSP_FILTER_FIR_REAL
#define RAPTORDSP_FILTER_FIR_REAL

class raptor_filter_real {

public:
    raptor_filter_real();
    void configure(float* coeffs, int coeffsCount, int decimation);
    int process(float* input, float* output, int count, int channels);

private:
    float* coeffsBufferPtr;
    float* insampBufferPtr;
    float* insampBufferPtrOffset;
    float* tempBufferPtr; //used to keep VOLK aligned
    int taps;
    int decimation;
    int decimationIndex;
    int offset;

};


#endif
