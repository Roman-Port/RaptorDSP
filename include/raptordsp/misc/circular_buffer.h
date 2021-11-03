#ifndef ANDROIDSDRTUNER_CIRCULAR_BUFFER_H
#define ANDROIDSDRTUNER_CIRCULAR_BUFFER_H

#include <raptordsp/defines.h>

/***
 * A circular buffer object.
 * NOTE: THIS OBJECT IS NOT THREAD SAFE ON IT'S OWN!
 */
template <typename T>
class circular_buffer {

public:
    /***
     * Creates a circular array.
     * @param count The initial float count.
     */
    circular_buffer(int count) : buffer(nullptr), use(0), size(count), pos_read(0), pos_write(0) {
        //Allocate buffer
        buffer = (T*)malloc(sizeof(T) * count);
        assert(buffer != nullptr);
    }

    ~circular_buffer() {
        free(buffer);
    }

    /***
     * Gets the total size of the buffer, in floats.
     */
    int get_size() {
        return size;
    }

    /***
     * Gets the usage of the buffer, in floats.
     */
    int get_used() {
        return use;
    }

    /***
     * Gets the free space, in floats.
     */
    int get_free() {
        return get_size() - get_used();
    }

    /***
     * Resizes the buffer, optionally keeping the data. Expensive operation.
     * @param count The new size of the buffer, in floats.
     * @param keepContents If true, the old contents are copied to the new buffer and it can be used seamlessly. Otherwise, contents are lost and the buffer is reset.
     */
    void resize(int newSize, bool keepContents) {
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
        } else {
            //We don't care about the previous buffer, reset...
            pos_write = 0;
        }

        //Reset everything
        buffer = newBuffer;
        size = newSize;
        pos_read = 0;
    }

    /***
     * Resets the buffer, removing all content in it.
     */
    void clear() {
        pos_write = 0;
        pos_read = 0;
        use = 0;
    }

    /***
     * Writes to the circular buffer.
     * @param input The buffer to copy in.
     * @param count The number of items to write, in floats.
     * @param extend If true, the buffer will seamlessly expand to fit all the items. Otherwise, existing items will be overwritten.
     */
    void write(T* input, int incoming, bool extend) {
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

    /***
     * Reads out of the circular buffer.
     * @param output The buffer to copy to.
     * @param count The number of items to read at MAXIMUM, in floats.
     * @return The actual number of items read, in floats.
     */
    int read(T* output, int outputRemaining) {
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

private:
    T* buffer;
    int size;
    int use;
    int pos_write;
    int pos_read;

};

#endif //ANDROIDSDRTUNER_CIRCULAR_BUFFER_H
