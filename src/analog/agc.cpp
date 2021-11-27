#include <raptordsp/analog/agc.h>

template <class T>
raptor_agc<T>::raptor_agc(float rate, float reference, float gain, float max_gain) :
    _rate(rate),
    _reference(reference),
    _gain(gain),
    _max_gain(max_gain)
{

}

template <class T>
void raptor_agc<T>::process(const T* input, T* output, int count) {
    for (int i = 0; i < count; i++) {
        //Process one
        output[i] = process_one(input[i]);

        //Wrap
        if (_max_gain > 0.0 && _gain > _max_gain)
            _gain = _max_gain;
    }
}

template class raptor_agc<float>;
template class raptor_agc<raptor_complex>;

template <>
float raptor_agc_float::process_one(float input) {
    float output = input * _gain;
    _gain += (_reference - fabsf(output)) * _rate;

    return output;
}

template <>
raptor_complex raptor_agc_complex::process_one(raptor_complex input) {
    raptor_complex output = input * _gain;
    _gain += _rate * (_reference - std::sqrt(output.real() * output.real() + output.imag() * output.imag()));

    return output;
}