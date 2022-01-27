#pragma once

#include <raptordsp/defines.h>

template<typename T>
class raptor_delay_line {

public:
    raptor_delay_line();
    raptor_delay_line(raptor_delay_line const& src);
    ~raptor_delay_line();

    void configure(int delay, T defaultValue);
    void process(const T* input, T* output, int count);

private:
    T* buffer;
    int len;
    int pos;

};

typedef raptor_delay_line<float> raptor_delay_line_float;
typedef raptor_delay_line<raptor_complex> raptor_delay_line_complex;