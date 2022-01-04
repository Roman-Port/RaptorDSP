#pragma once

#include <raptordsp/misc/circular_buffer.h>
#include <cassert>
#include <cstring>

template <typename T>
circular_buffer<T>::circular_buffer(int count) : buffer(nullptr), use(0), size(count), pos_read(0), pos_write(0) {
    //Allocate buffer
    buffer = (T*)malloc(sizeof(T) * count);
    assert(buffer != nullptr);
}

template <typename T>
circular_buffer<T>::~circular_buffer() {
    free(buffer);
}

template <typename T>
int circular_buffer<T>::get_size() {
    return size;
}

template <typename T>
int circular_buffer<T>::get_used() {
    return use;
}

template <typename T>
int circular_buffer<T>::get_free() {
    return get_size() - get_used();
}

template <typename T>
void circular_buffer<T>::resize(int newSize, bool keepContents) {
    //If we aren't keeping contents, free the buffer right away
    if (!keepContents)
        free(buffer);

    //Allocate our new buffer
    T* newBuffer = (T*)malloc(sizeof(T) * newSize);
    assert(newBuffer != nullptr);

    //If we're keeping contents, unwrap the current buffer into this one
    if (keepContents) {
        //Validate
        assert(newSize >= use);

        //Unwrap contents over to the new buffer, relative to the reading pointer
        memcpy(newBuffer, &buffer[pos_read], sizeof(T) * (size - pos_read));
        memcpy(&newBuffer[pos_read], buffer, sizeof(T) * pos_read);

        //The reading pointer will now be at 0, but we need to figure out where the writing one will be
        pos_write -= pos_read;
        if (pos_write < 0) //If it falls under zero, wrap around
            pos_write += size;

        //Free our old buffer, as it's no longer needed
        free(buffer);
    }
    else {
        //We don't care about the previous buffer, reset...
        pos_write = 0;
    }

    //Reset everything
    buffer = newBuffer;
    size = newSize;
    pos_read = 0;
}

template <typename T>
void circular_buffer<T>::clear() {
    pos_write = 0;
    pos_read = 0;
    use = 0;
}

template <typename T>
void circular_buffer<T>::write(T* input, int incoming, bool extend) {
    //Make sure this buffer has enough space if we've requested it by resizing the buffer...
    while (extend && get_free() <= incoming)
        resize(get_size() * 2, true);

    //Assumes there's enough space; if there's not, data will be overwritten
    int writable;
    while (incoming > 0) {
        //Determine writable
        writable = std::min(incoming, size - pos_write);

        //Write to the buffer
        memcpy(&buffer[pos_write], input, sizeof(T) * writable);

        //Update state
        input += writable;
        incoming -= writable;
        pos_write = (pos_write + writable) % size;
        use += writable;

        //If our usage is over our length, force push the read location forward
        if (use > size) {
            use = size;
            pos_read = pos_write;
        }
    }
}

template <typename T>
int circular_buffer<T>::try_write(T* input, int incoming, bool allOrNothing) {
    //Make sure there's enough space for everything if we set allOrNothing
    if (allOrNothing && incoming > get_free())
        return 0;

    //Limit the number of incoming items to the size of the buffer
    if (incoming > get_free())
        incoming = get_free();

    //Write these as normal
    write(input, incoming, false);

    return incoming;
}

template <typename T>
int circular_buffer<T>::read(T* output, int outputRemaining) {
    int read = 0;
    int readable;
    while (outputRemaining > 0 && use > 0) {
        //Determine how much is readable
        readable = std::min(outputRemaining, std::min(size - pos_read, use));

        //Write to the output
        memcpy(output, &buffer[pos_read], sizeof(T) * readable);

        //Update state
        output += readable;
        outputRemaining -= readable;
        read += readable;
        pos_read = (pos_read + readable) % size;
        use -= readable;
    }
    return read;
}

template class circular_buffer<float>;
template class circular_buffer<raptor_complex>;