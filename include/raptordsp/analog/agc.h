#pragma once

#include <raptordsp/defines.h>

template <class T>
class raptor_agc {

public:
	raptor_agc(float rate = 1e-4, float reference = 1.0, float gain = 1.0, float max_gain = 0.0);
    void process(const T* input, T* output, int count);

private:
    T process_one(T input);

    float _rate;      // adjustment rate
    float _reference; // reference value
    float _gain;      // current gain
    float _max_gain;  // max allowable gain

};

typedef raptor_agc<float> raptor_agc_float;
typedef raptor_agc<raptor_complex> raptor_agc_complex;