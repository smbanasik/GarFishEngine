/**
* @file
* @author Spencer Banasik
* @relates RefCounter
*/
#ifndef COM_REFCOUNTER_HPP
#define COM_REFCOUNTER_HPP

#include <stdint.h>

namespace com {
/**
 * @brief A reference counter
 * @author Spencer Banasik
 * @details My implementation of a reference counter, which
 * would be something similar to that in a shared pointer.
 */
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

    /**
     * @brief Returns whether it is safe to delete associated resource.
     * @author Spencer Banasik
     * @returns A boolean on if there is only one reference or not.
     */
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
#endif


