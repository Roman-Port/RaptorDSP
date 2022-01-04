#pragma once

#include <raptordsp/misc/circular_buffer.h>
#include <mutex>
#include <condition_variable>

/***
 * A circular buffer object, thread safe.
 */
template <typename T>
class circular_buffer_safe {

public:
    /***
     * Creates a circular array.
     * @param count The initial float count.
     */
    circular_buffer_safe(int count);

    /***
     * Resizes the buffer, optionally keeping the data. Expensive operation.
     * @param count The new size of the buffer, in floats.
     * @param keepContents If true, the old contents are copied to the new buffer and it can be used seamlessly. Otherwise, contents are lost and the buffer is reset.
     */
    void resize(int count, bool keepContents);

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
    void write(T* input, int count, bool extend);

    /// <summary>
    /// Writes to the circular buffer only if there's enough space. Does not overwrite items or extend the buffer.
    /// </summary>
    /// <param name="input"></param>
    /// <param name="count"></param>
    /// <param name="allOrNothing">If set to true, no items will be written unless there's enough space to write EVERYTHING.</param>
    /// <returns></returns>
    int try_write(T* input, int count, bool allOrNothing);

    /***
     * Reads out of the circular buffer.
     * @param output The buffer to copy to.
     * @param count The number of items to read at MAXIMUM, in floats.
     * @return The actual number of items read, in floats.
     */
    int read(T* output, int count);

    /***
     * Reads out of the circular buffer. Hangs until samples are available.
     * @param output The buffer to copy to.
     * @param count The number of items to read at MAXIMUM, in floats.
     * @return The actual number of items read, in floats.
     */
    int read_wait(T* output, int count);

    /***
     * Cancels an ongoing read that we're waiting on.
     */
    void read_cancel();

private:
    circular_buffer<T> underlying;
    bool cancel;
    std::mutex m;
    std::condition_variable cv;

};

typedef circular_buffer_safe<float> circular_buffer_safe_float;
typedef circular_buffer_safe<raptor_complex> circular_buffer_safe_complex;