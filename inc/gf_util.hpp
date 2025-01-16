// Spencer Banasik
// Created: 1/8/2025
// Last Modified: 1/10/2025
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
    }
    RefCounter(const RefCounter& other)
        : counter(other.counter) {
        (*counter)++;
    }
    RefCounter& operator=(const RefCounter& other) {
        if (this == &other)
            return *this;

        decrement_delete_counter();
        counter = other.counter;
        (*counter)++;
        return *this;
    }
    RefCounter(RefCounter&& other) noexcept
        : counter(other.counter) {
        other.counter = nullptr;
    }
    RefCounter& operator=(RefCounter&& other) noexcept {
        decrement_delete_counter();
        counter = other.counter;
        other.counter = nullptr;
        return *this;
    }

    bool can_delete_resources() const {
        if (counter != nullptr && *counter == 1) // If counter is null somehow, it means resources were moved.
            return true;
        return false;
    }

private:
    void decrement_delete_counter() {
        if (counter == nullptr)
            return;
        (*counter)--;
        if (*counter == 0)
            delete counter;
    }

    uint32_t* counter;
};
}
}
#endif