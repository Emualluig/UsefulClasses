#ifndef __HEADER_oSTRING_H_
#define __HEADER_oSTRING_H_

#include <utility>

// Re-implemented small string optimization
static constexpr const uint64_t ARRAY_SIZE = (sizeof(unsigned char*) + sizeof(uint64_t) + sizeof(uint64_t) - sizeof(unsigned char)) / sizeof(unsigned char);
static constexpr const uint64_t ALLOC_SIZE = 32;
static constexpr unsigned char BIT_MASK_CHAR = static_cast<unsigned char>(1) << (sizeof(unsigned char) * 8 - 1);
static constexpr uint64_t BIT_MASK_UINT = static_cast<uint64_t>(1) << (sizeof(uint64_t) * 8 - 1);
class oString {
    struct Small {
        unsigned char array[ARRAY_SIZE];
        unsigned char length;
    };
    struct Large {
        // The array will always be a power of 2 (starts at 32, 64, 128, ...)
        unsigned char* array;
        uint64_t capacity;
        uint64_t length;
    };
    struct _string {
        union {
            Small _small;
            Large _large;
        };
    };

    bool isSmall() const noexcept {
        return (str._small.length & BIT_MASK_CHAR) == 0;
    }
    bool isLarge() const noexcept {
        return (str._small.length & BIT_MASK_CHAR) != 0;
    }
    void increment() noexcept {
        const uint64_t currentSize = size();
        if (currentSize < ARRAY_SIZE) {
            str._small.length -= 1;
        }
        else if (currentSize == ARRAY_SIZE) {
            // We change from counting down to 0 to counting up
            str._large.length = BIT_MASK_UINT + ARRAY_SIZE + 1;
        }
        else {
            str._large.length += 1;
        }
    }
    // If you decrement on zero, it becomes invalidated
    void decrement() noexcept {
        const uint64_t currentSize = size();
        if (currentSize <= ARRAY_SIZE) {
            str._small.length += 1;
        }
        else if (currentSize == ARRAY_SIZE + 1) {
            str._large.length = ARRAY_SIZE;
        }
        else {
            str._large.length -= 1;
        }
    }

public:
    _string str;
    explicit oString() noexcept {
        str = {
            0,0,0,0,0,0,
            0,0,0,0,0,0,
            0,0,0,0,0,0,
            0,0,0,0,0,ARRAY_SIZE
        };
    }
    explicit oString(const char* literal) noexcept {
        uint64_t length = 0;
        while (literal[length] != '\0') {
            length++;
        }
        if (length <= ARRAY_SIZE) {
            // load into small string
            str._small.length = ARRAY_SIZE;
            for (uint64_t i = 0; i < length; i++) {
                str._small.array[i] = literal[i];
                increment();
            }
            for (uint64_t i = length; i < ARRAY_SIZE; i++) {
                str._small.array[i] = '\0';
            }
        }
        else {
            // Load into large string
            str._large.length = BIT_MASK_UINT + length;
            str._large.capacity = ((length / ALLOC_SIZE) + 1) * ALLOC_SIZE; // Allocate to nearest power of 2 greater or equal to ALLOC_SIZE
            unsigned char* tmpArray = new unsigned char[str._large.capacity];
            for (uint64_t i = 0; i < length; i++) {
                tmpArray[i] = literal[i];
            }
            tmpArray[length] = '\0';
            str._large.array = tmpArray;
        }
    }
    ~oString() noexcept {
        if (!isSmall()) {
            // This is dangerous. If the size get an invalid value we might delete stack memory, resulting in a crash.
            delete[] str._large.array;
        }
    }
    const char* c_str() const noexcept {
        if (isSmall()) {
            return (const char*)this;
        }
        return (const char*)str._large.array;
    }
    uint64_t size() const noexcept {
        if (isSmall()) {
            return ARRAY_SIZE - str._small.length;
        }
        return str._large.length & ~BIT_MASK_UINT;
    }

    // push_back(value) consumes a character and appends it to the end of the string
    // time: O(1) amortized
    // effects: may allocate memory
    void push_back(char value) noexcept {
        const uint64_t currentSize = size();
        if (currentSize < ARRAY_SIZE) {
            str._small.array[currentSize] = value;
            str._small.length -= 1;
        }
        else if (currentSize == ARRAY_SIZE) {
            // We are switching to using large mode
            unsigned char* tmpArray = new unsigned char[ALLOC_SIZE];
            for (int i = 0; i < ARRAY_SIZE; i++) {
                tmpArray[i] = str._small.array[i];
            }
            tmpArray[ARRAY_SIZE] = value;
            tmpArray[ARRAY_SIZE + 1] = '\0';

            str._large.length = BIT_MASK_UINT + ARRAY_SIZE + 1;
            str._large.capacity = ALLOC_SIZE;
            str._large.array = tmpArray;
        }
        else {
            if (currentSize + 1 >= str._large.capacity) {
                // Double allocated capacity if we run out of space
                str._large.capacity *= 2;
                unsigned char* tmpArray = new unsigned char[str._large.capacity];
                for (int i = 0; i < currentSize; i++) {
                    tmpArray[i] = str._large.array[i];
                }
                delete[] str._large.array;
                str._large.array = tmpArray;
            }
            // There is now enough memory to write another character and null terminator
            str._large.array[currentSize] = value;
            str._large.array[currentSize + 1] = '\0';
            str._large.length += 1;
        }
    }
    // TODO: implement operator==
    //                 operator[]
    //                 .at()
    //                 compare
    //                 pop_back()
    //                 operator>>
    //                 operator<<
    //                 hashing
    //                 operator+=
    //                 append
    //                 copy (creates a copy of this string)
    //                 to_string() converts to a std::string
    //                 oString(std::string) creates a oString from std::string
};

#endif // !__HEADER_oSTRING_H_

