// Spencer Banasik
// Created: 1/8/2025
// Last Modified: 1/8/2025
// Description:
// Utility structures for the engine
#ifndef GF_UTIL_HPP
#define GF_UTIL_HPP

#include <stdint.h>

namespace gf {
namespace util {
class RefCounter {
public:

    RefCounter() 
    : counter(new uint32_t) {
        *counter = 1;
    }
    ~RefCounter() {
        decrement_delete_counter();
        counter = nullptr;
    }
    RefCounter(RefCounter& other)
    : counter(other.counter) {
        (*counter)++;
    }
    RefCounter& operator=(RefCounter& other) {
        if (this == &other)
            return;

        decrement_delete_counter();
        counter = other.counter;
        (*counter)++;
    }
    RefCounter(RefCounter&& other) noexcept 
    : counter(other.counter) {
        other.counter = nullptr;
    }
    RefCounter& operator=(RefCounter&& other) noexcept {
        counter = other.counter;
        other.counter = nullptr;
    }

    uint32_t get_count() { return *counter; };

private:
    void decrement_delete_counter() {
        (*counter)--;
        if (*counter = 0)
            delete counter;
    }

    uint32_t* counter;
};
}
}
#endif