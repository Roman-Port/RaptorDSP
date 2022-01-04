#pragma once

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
    circular_buffer(int count);
    ~circular_buffer();

    /***
     * Gets the total size of the buffer, in floats.
     */
    int get_size();

    /***
     * Gets the usage of the buffer, in floats.
     */
    int get_used();

    /***
     * Gets the free space, in floats.
     */
    int get_free();

    /***
     * Resizes the buffer, optionally keeping the data. Expensive operation.
     * @param count The new size of the buffer, in floats.
     * @param keepContents If true, the old contents are copied to the new buffer and it can be used seamlessly. Otherwise, contents are lost and the buffer is reset.
     */
    void resize(int newSize, bool keepContents);

    /***
     * Resets the buffer, removing all content in it.
     */
    void clear();

    /***
     * Writes to the circular buffer.
     * @param input The buffer to copy in.
     * @param count The number of items to write, in floats.
     * @param extend If true, the buffer will seamlessly expand to fit all the items. Otherwise, existing items will be overwritten.
     */
    void write(T* input, int incoming, bool extend);

    /// <summary>
    /// Writes to the circular buffer only if there's enough space. Does not overwrite items or extend the buffer.
    /// </summary>
    /// <param name="input"></param>
    /// <param name="incoming"></param>
    /// <param name="allOrNothing">If set to true, no items will be written unless there's enough space to write EVERYTHING.</param>
    /// <returns></returns>
    int try_write(T* input, int incoming, bool allOrNothing);

    /***
     * Reads out of the circular buffer.
     * @param output The buffer to copy to.
     * @param count The number of items to read at MAXIMUM, in floats.
     * @return The actual number of items read, in floats.
     */
    int read(T* output, int outputRemaining);

private:
    T* buffer;
    int size;
    int use;
    int pos_write;
    int pos_read;

};

typedef circular_buffer<float> circular_buffer_float;
typedef circular_buffer<raptor_complex> circular_buffer_complex;
