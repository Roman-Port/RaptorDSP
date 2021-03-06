#include <raptordsp/misc/circular_buffer_safe.h>
#include <cstring>

template <typename T>
circular_buffer_safe<T>::circular_buffer_safe(int count) : underlying(count), cancel(false) {

}

/***
 * Resizes the buffer, optionally keeping the data. Expensive operation.
 * @param count The new size of the buffer, in floats.
 * @param keepContents If true, the old contents are copied to the new buffer and it can be used seamlessly. Otherwise, contents are lost and the buffer is reset.
 */
template <typename T>
void circular_buffer_safe<T>::resize(int count, bool keepContents) {
    m.lock();
    underlying.resize(count, keepContents);
    m.unlock();
}

/***
 * Resets the buffer, removing all content in it.
 */
template <typename T>
void circular_buffer_safe<T>::clear() {
    m.lock();
    underlying.clear();
    m.unlock();
}

/***
 * Writes to the circular buffer.
 * @param input The buffer to copy in.
 * @param count The number of items to write, in floats.
 * @param extend If true, the buffer will seamlessly expand to fit all the items. Otherwise, existing items will be overwritten.
 */
template <typename T>
void circular_buffer_safe<T>::write(T* input, int count, bool extend) {
    m.lock();
    underlying.write(input, count, extend);
    m.unlock();
    cv.notify_one();
}

/// <summary>
/// Writes to the circular buffer only if there's enough space. Does not overwrite items or extend the buffer.
/// </summary>
/// <param name="input"></param>
/// <param name="count"></param>
/// <param name="allOrNothing">If set to true, no items will be written unless there's enough space to write EVERYTHING.</param>
/// <returns></returns>
template <typename T>
int circular_buffer_safe<T>::try_write(T* input, int count, bool allOrNothing) {
    m.lock();
    count = underlying.try_write(input, count, allOrNothing);
    m.unlock();
    cv.notify_one();
    return count;
}

/***
 * Reads out of the circular buffer.
 * @param output The buffer to copy to.
 * @param count The number of items to read at MAXIMUM, in floats.
 * @return The actual number of items read, in floats.
 */
template <typename T>
int circular_buffer_safe<T>::read(T* output, int count) {
    m.lock();
    int read = underlying.read(output, count);
    m.unlock();
    return read;
}

/***
 * Reads out of the circular buffer. Hangs until samples are available.
 * @param output The buffer to copy to.
 * @param count The number of items to read at MAXIMUM, in floats.
 * @return The actual number of items read, in floats.
 */
template <typename T>
int circular_buffer_safe<T>::read_wait(T* output, int count) {
    //Wait
    std::unique_lock<std::mutex> lk(m);
    while (!cancel && underlying.get_used() == 0)
        cv.wait(lk);

    //Perform the read
    int read;
    if (!cancel)
        read = underlying.read(output, count);
    else
        read = 0;

    //Reset cancellation
    cancel = false;

    //Unlock
    lk.unlock();

    return read;
}

/***
 * Cancels an ongoing read that we're waiting on.
 */
template <typename T>
void circular_buffer_safe<T>::read_cancel() {
    cancel = true;
    cv.notify_one();
}

template class circular_buffer_safe<float>;
template class circular_buffer_safe<raptor_complex>;