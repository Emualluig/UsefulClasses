#ifndef __DECTOR_CLASS_H_
#define __DECTOR_CLASS_H_

#include <cstdlib>
#include <utility>

// dector provides a double ended vector that supports O(1) additions to both the front and back
// All memory is also stored contiguously
template<typename T> class dector {
    // Required: left <= right
    std::size_t left;
    std::size_t right;
    std::size_t capacity;
    T* array;

    // Conditionally reallocate the array if there is not enough space at the front
    void realloc_front() noexcept {
        if (left == 0) {
            // Not enough space, must reallocate
            T* tmp = array;
            std::size_t oldCapacity = capacity;
            capacity *= 2;
            array = new T[capacity];

            // Realloc'ing at the front means the first element is no longer at index 0
            // So we offset in the new array by the old capacity
            for (std::size_t index = left; index < right; index++) {
                array[oldCapacity + index] = std::move(tmp[index]);
            }
            left += oldCapacity;
            right += oldCapacity;
            delete[] tmp;
        }
    }
    // Conditionally reallocate the array if there is not enough space at the back
    void realloc_back() noexcept {
        if (right == capacity) {
            // Not enough space, must reallocate
            T* tmp = array;
            std::size_t oldCapacity = capacity;
            capacity *= 2;
            array = new T[capacity];

            // Realloc'ing in the back doesn't require an offset
            for (std::size_t index = left; index < right; index++) {
                array[index] = std::move(tmp[index]);
            }
            delete[] tmp;
        }
    }
public:
    dector() noexcept : left{ 5 }, right{ 5 }, capacity{ 11 } {
        array = new T[capacity];
    }
    ~dector() noexcept {
        delete[] array;
    }
    void push_front(const T& value) noexcept {
        realloc_front();
        left -= 1;
        array[left] = value;
    }
    void push_front(T&& value) noexcept {
        realloc_front();
        left -= 1;
        array[left] = std::move(value);
    }
    void push_back(const T& value) noexcept {
        realloc_back();
        array[right] = value;
        right += 1;
    }
    void push_back(T&& value) noexcept {
        realloc_back();
        array[right] = std::move(value);
        right += 1;
    }
    void pop_front() noexcept {
        left += 1;
    }
    void pop_back() noexcept {
        right -= 1;
    }
    void set(std::size_t index, const T& value) noexcept {
        array[index] = value;
    }
    void set(std::size_t index, T&& value) noexcept {
        array[index] = std::move(value);
    }
    T& at_refr(std::size_t index) const noexcept {
        return array[index + left];
    }
    T at_copy(std::size_t index) const noexcept {
        return array[index + left];
    }
    void condense() noexcept {
        constexpr std::size_t marginSize = 5;
        std::size_t currentSize = size();
        T* tmp = array;
        capacity = currentSize + 2 * marginSize;
        array = new T[capacity];
        for (std::size_t index = left; index < right; index++) {
            array[marginSize + index] = std::move(tmp[index]);
        }
        left = marginSize;
        right = marginSize + currentSize;

        delete[] tmp;
    }
    std::size_t size() const noexcept {
        return right - left;
    }
};

#endif // !__DECTOR_CLASS_H_

