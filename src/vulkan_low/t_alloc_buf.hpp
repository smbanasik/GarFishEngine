/**
 * @file
 * @author Spencer Banasik
 */
#ifndef T_ALLOC_BUF_HPP
#define T_ALLOC_BUF_HPP

#include <utility>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <t_refcount.hpp>

namespace vkl_res {

class ImageBufferAllocator;

/**
 * @struct AllocatedImage
 * @brief Contains all necessary data to allocate a buffer.
 * @author Spencer Banasik
 * @details A RAII wrapper of a buffer, it's allocation,
 * and additional info. AllocatedBuffers are reference counted,
 * similar to shared pointers.
 */
struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
    com::RefCounter counter;
    const ImageBufferAllocator* allocator;

    AllocatedBuffer()
        : buffer(nullptr),
        allocation(nullptr),
        info(),
        counter(),
        allocator(nullptr) {}
    AllocatedBuffer(const ImageBufferAllocator& allocator)
        : buffer(nullptr),
        allocation(nullptr),
        info(),
        counter(),
        allocator(&allocator) {}
    AllocatedBuffer(const AllocatedBuffer& other);
    AllocatedBuffer(AllocatedBuffer&& other) noexcept;
    AllocatedBuffer& operator=(const AllocatedBuffer& other) {
        if (this == &other)
            return *this;
        buffer = other.buffer;
        allocation = other.allocation;
        info = other.info;
        counter = other.counter;
        allocator = other.allocator;
        return *this;
    }
    AllocatedBuffer& operator=(AllocatedBuffer&& other) noexcept {
        if (this == &other)
            return *this;
        buffer = std::move(other.buffer);
        allocation = std::move(other.allocation);
        info = std::move(other.info);
        counter = std::move(other.counter);
        allocator = std::move(other.allocator);
        other.buffer = nullptr;
        other.allocation = nullptr;
        return *this;
    }
    ~AllocatedBuffer();
};
}
#endif
