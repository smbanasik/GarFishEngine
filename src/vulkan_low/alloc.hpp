/**
 * @file
 * @relates Allocator
 */
#ifndef VKL_ALLOC_HPP
#define VKL_ALLOC_HPP

#include <vk_mem_alloc.h>

#include <vulkan_low/core.hpp>

namespace vkl_core {
/**
* @class VMAAllocWrapper
* @brief Logical abstraction of vma allocator with RAII.
* @author Spencer Banasik
* @details Contains a vma allocator with the rule of 5.
* Copying is not permitted, but has move operations available.
*/
class VMAAllocWrapper {
public:
    /**
    * @brief Construct the allocator with information from VKCore.
    * @author Spencer Banasik
    */
    VMAAllocWrapper(VKCore* core) {
        VmaAllocatorCreateInfo allocator_info = {};
        allocator_info.physicalDevice = core->gpu;
        allocator_info.device = core->device;
        allocator_info.instance = core->instance;
        allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocator_info, &allocator);
    }
    ~VMAAllocWrapper() {
        if (allocator != nullptr)
            vmaDestroyAllocator(allocator);
    }
    VMAAllocWrapper(VMAAllocWrapper&& other) noexcept
        : allocator(std::move(other.allocator)) {
        other.allocator = nullptr;
    }
    VMAAllocWrapper& operator=(VMAAllocWrapper&& other) noexcept {
        allocator = std::move(other.allocator);
        other.allocator = nullptr;
    }
    VMAAllocWrapper(VMAAllocWrapper& other) = delete;
    VMAAllocWrapper& operator=(VMAAllocWrapper& other) = delete;

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