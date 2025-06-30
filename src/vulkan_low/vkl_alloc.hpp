/**
 * @file
 * @relates Allocator
 */
#ifndef VKL_ALLOC_HPP
#define VKL_ALLOC_HPP

#include <vk_mem_alloc.h>

#include <vkl_core.hpp>

namespace vkl_core {
/**
* @class Allocator
* @brief Logical abstraction of vma allocator with RAII.
* @author Spencer Banasik
* @details Contains a vma allocator with the rule of 5.
* Copying is not permitted, but has move operations available.
*/
class Allocator {
public:
    /**
    * @brief Construct the allocator with information from VKCore.
    * @author Spencer Banasik
    */
    Allocator(VKCore* core) {
        VmaAllocatorCreateInfo allocator_info = {};
        allocator_info.physicalDevice = core->gpu;
        allocator_info.device = core->device;
        allocator_info.instance = core->instance;
        allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocator_info, &allocator);
    }
    ~Allocator() {
        if (allocator != nullptr)
            vmaDestroyAllocator(allocator);
    }
    Allocator(Allocator&& other) noexcept
        : allocator(std::move(other.allocator)) {
        other.allocator = nullptr;
    }
    Allocator& operator=(Allocator&& other) noexcept {
        allocator = std::move(other.allocator);
        other.allocator = nullptr;
    }
    Allocator(Allocator& other) = delete;
    Allocator& operator=(Allocator& other) = delete;

    /**
     * @brief Allocator from VMA
     * @details VMA is an easy way to avoid creating our own method of
     * allocating objects in vulkan, which can be very error prone.
     * This is used to allocate vulakn objects such as images and buffers.
     */
    VmaAllocator allocator = nullptr;
};
}
#endif