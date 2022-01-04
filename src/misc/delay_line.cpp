#include <raptordsp/misc/delay_line.h>
#include <cstring>

template<typename T>
raptor_delay_line<T>::raptor_delay_line(int delay, T defaultValue) {
    //Allocate the buffer
    buffer = (T*)malloc(sizeof(T) * delay);

    //Zero out buffer
    for (int i = 0; i < delay; i++)
        buffer[i] = defaultValue;

    //Set up vars
    pos = 0;
    len = delay;
}

template<typename T>
raptor_delay_line<T>::raptor_delay_line(raptor_delay_line const& src) {
    len = src.len;
    pos = src.pos;
    buffer = (T*)malloc(sizeof(T) * len);
    memcpy(buffer, src.buffer, sizeof(T) * len);
}

template<typename T>
raptor_delay_line<T>::~raptor_delay_line() {
    free(buffer);
    buffer = nullptr;
}

template<typename T>
void raptor_delay_line<T>::process_one(T input, T* output) {
    //Read output
    *output = buffer[pos];

    //Write value in its place
    buffer[pos] = input;

    //Advance pointer
    pos = (pos + 1) % len;
}

template<typename T>
void raptor_delay_line<T>::process(T* ptr, int count) {
    T temp;
    for (int i = 0; i < count; i++) {
        process_one(ptr[i], &temp);
        ptr[i] = temp;
    }
}

template class raptor_delay_line<float>;
template class raptor_delay_line<raptor_complex>;