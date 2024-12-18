// Spencer Banasik
// Created: 12/15/2024
// Last Modified: 12/18/2024
// Description:
// Contains various types and helper functions for the engine
#ifndef GF_ENGINE_TYPES_HPP
#define GF_ENGINE_TYPES_HPP

#include <vector>
#include <deque>
#include <functional>

namespace gf {

// Stores deletion functions, prefer deleting arrays in loops as opposed to storing callbacks like this
// method does. Primarily used for cleaning up global resources
class DeletionStack {
public:
    void push_function(std::function<void()> && function) {
        deletors.push_front(function);
    }
    void flush() {
        for (auto it = deletors.begin(); it != deletors.end(); it++) {
            (*it)();
        }
        deletors.clear();
    }

private:
    std::deque<std::function<void()>> deletors;
};

template<class T>
class DeletionAggregate {
public:

    DeletionAggregate(std::function<void()> func, uint32_t elems = 0) : deletion(func) {
        aggregate.reserve(elems);
    };
    void push_elem(T elem) {
        aggregate.push_back(elem);
    }
    void flush() {
        deletion();
    }

private:
    std::vector<T> aggregate;
    std::function<void()> deletion;
};
}
#endif
