#include <raptordsp/misc/delay_line.h>
#include <cstring>
#include <cassert>

template<typename T>
raptor_delay_line<T>::raptor_delay_line() :
    buffer(0),
    pos(0),
    len(-1)
{
}

template<typename T>
raptor_delay_line<T>::raptor_delay_line(raptor_delay_line const& src) {
    //Initialize defaults
    buffer = src.buffer;
    pos = src.pos;
    len = src.len;

    //If the source has a buffer, copy it to our own
    if (src.buffer != 0) {
        //Create
        buffer = (T*)malloc(sizeof(T) * len);
        assert(buffer != 0);

        //Copy
        memcpy(buffer, src.buffer, sizeof(T) * len);
    }
}

template<typename T>
raptor_delay_line<T>::~raptor_delay_line() {
    //Destroy the old buffer
    if (buffer != 0) {
        free(buffer);
        buffer = 0;
    }
}

template<typename T>
void raptor_delay_line<T>::configure(int newDelay, T defaultValue) {
    //Sanity check
    assert(newDelay >= 0);

    //Only resize buffer if we need to
    if (len != newDelay || buffer == 0) {
        //Destroy the old buffer
        if (buffer != 0) {
            free(buffer);
            buffer = 0;
        }

        //Create new buffer if needed
        if (newDelay > 0) {
            buffer = (T*)malloc(newDelay * sizeof(T));
            assert(buffer != 0);
        }
    }

    //Reset state
    len = newDelay;
    pos = 0;

    //Clear buffer
    for (int i = 0; i < newDelay; i++)
        buffer[i] = defaultValue;
}

template<typename T>
void raptor_delay_line<T>::process(const T* input, T* output, int count) {
    //Sanity check
    assert(count >= 0);

    //Check if we need to shortcut this
    if (len == 0) {
        //No delay. Just copy
        memcpy(output, input, sizeof(T) * count);
    }
    else {
        //Sanity check
        assert(buffer != 0);

        //Process delay
        T temp;
        for (int i = 0; i < count; i++) {
            temp = buffer[pos];
            buffer[pos++] = input[i];
            pos %= len;
            output[i] = temp;
        }
    }
}

template class raptor_delay_line<float>;
template class raptor_delay_line<raptor_complex>;