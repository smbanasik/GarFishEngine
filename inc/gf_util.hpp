// Spencer Banasik
// Created: 1/8/2025
// Last Modified: 1/10/2025
// Description:
// Utility structures for the engine
#ifndef GF_UTIL_HPP
#define GF_UTIL_HPP

#include <stdint.h>
#include <cassert>
#include <stdexcept>

#include <glm/vec2.hpp>

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
// Grid
// Given N,M, and T, create a percieved grid of type T and size NxM,
// but stored contiguously. Resizing the row width will truncate.
// Adding a row with insufficient elements is undefined.
template<typename T>
class Grid {
public:
    Grid(std::size_t row_size)
        : v_row_size(row_size), v_size(0),
        v_capacity(0), data_buffer(nullptr) {
        assert(row_size > 0);
    }
    Grid(const Grid& other)
        : v_row_size(other.v_row_size),
        v_size(other.v_size),
        v_capacity(other.v_capacity)
    {
        other.copy(this, 0, 0, v_size);
    }
    Grid(Grid&& other) noexcept
        : v_row_size(std::move(other.v_row_size)),
        v_size(std::move(other.v_size)),
        v_capacity(std::move(other.v_capacity)),
        data_buffer(std::move(other.data_buffer)) {
        other.data_buffer = nullptr;
    }
    Grid& operator=(const Grid& other) {
        if (this == &other)
            return *this;
        v_row_size = other.v_row_size;
        v_size = other.v_size;
        v_capacity = other.v_capacity;
        other.copy(this, 0, 0, v_size);
        return *this;
    }
    Grid& operator=(Grid&& other) noexcept {
        if (this == &other)
            return *this;
        v_row_size = std::move(other.v_row_size);
        v_size = std::move(other.v_size);
        v_capacity = std::move(other.v_capacity);
        data_buffer = std::move(other.data_buffer);
        other.data_buffer = nullptr;
        return *this;
    }
    ~Grid() {
        delete[] data_buffer;
    }

    void reserve_rows(std::size_t num_rows) {
        if (num_rows > SIZE_MAX / v_row_size)
            throw std::overflow_error("Grid capacity would exceed maximum!");
        std::size_t new_capacity = v_row_size * num_rows;
        std::size_t new_size = (new_capacity < v_size) ? new_capacity : v_size;
        T* new_buffer = new T[new_capacity];
        for (size_t i = 0; i < new_size; i++) {
            new_buffer[i] = data_buffer[i];
        }
        v_capacity = new_capacity;
        v_size = new_size;
        delete[] data_buffer;
        data_buffer = new_buffer;
    }
    void resize_rows(std::size_t new_row_size) {
        assert(new_row_size != 0);
        v_row_size = new_row_size;
        if (v_size % v_row_size != 0)
            v_size = v_size - (v_size % v_row_size);
    }
    void push_back_row(T* arr) {
        if (v_size >= v_capacity)
            reserve_rows(num_rows() * 1.5);
        for (std::size_t i = 0; i < v_row_size; i++)
            data_buffer[i + v_size] = arr[i];
        v_size += v_row_size;
    }
    void add_empty_row() {
        if (v_size >= v_capacity)
            reserve_rows(std::size_t(num_rows() * 1.5));
        for (std::size_t i = 0; i < v_row_size; i++) {
            T new_elem{};
            data_buffer[i + v_size] = new_elem;
        }
        v_size += v_row_size;
    }
    void clear() {
        v_size = 0;
        delete[] data_buffer;
        data_buffer = nullptr;
    }
    void copy(Grid& dst, std::size_t dst_start, std::size_t source_start, std::size_t count) {
        assert(source_start + count <= v_size);
        if (dst.v_capacity + dst_start < count)
            dst.reserve_rows(count + dst_start);
        for (std::size_t i = 0; i < count; i++)
            dst[i + dst_start] = (*this)[i + source_start];
    }
    T& get_elem(glm::ivec2 column_row) {
        // TODO: throw index out of range
        return *(data_buffer + column_row.x + (column_row.y * v_row_size));
    }
    T& get_elem(std::size_t index) {
        return *(data_buffer + index);
    }
    T* data() { return data_buffer; }
    std::size_t row_size() {
        return v_row_size;
    }
    std::size_t size() {
        return v_capacity;
    }
    std::size_t num_rows() {
        return v_size / v_row_size;
    }
    T& operator[] (std::size_t index) {
        return *(data_buffer + index);
    }
    T& operator[] (glm::ivec2 column_row) {
        return *(data_buffer + column_row.x + (column_row.y * v_row_size));
    }

private:
    std::size_t v_row_size;
    std::size_t v_size;
    std::size_t v_capacity;
    T* data_buffer;
};


}
}
#endif