/**
 * @file
 * @relates Allocator
 */
#ifndef VKL_ALLOC_HPP
#define VKL_ALLOC_HPP

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace vkl_core {
class VkCore;
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
    VMAAllocWrapper(VkInstance instance, VkPhysicalDevice gpu, VkDevice device) {
        VmaAllocatorCreateInfo allocator_info = {};
        allocator_info.instance = instance;
        allocator_info.physicalDevice = gpu;
        allocator_info.device = device;
        allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocator_info, &allocator);
    }
    ~VMAAllocWrapper() {
        if (allocator != nullptr)
            vmaDestroyAllocator(allocator);
    }
    VMAAllocWrapper(VMAAllocWrapper&& other) noexcept : allocator(std::move(other.allocator)) {
        other.allocator = nullptr;
    }
    VMAAllocWrapper& operator=(VMAAllocWrapper&& other) noexcept {
        allocator = std::move(other.allocator);
        other.allocator = nullptr;
        return *this;
    }
    VMAAllocWrapper(VMAAllocWrapper& other) = delete;
    VMAAllocWrapper& operator=(VMAAllocWrapper& other) = delete;

    /**
     * @brief Allocator from VMA
     * @details VMA is an easy way to avoid creating our own method of
     * allocating objects in vulkan, which can be very error prone.
     * This is used to allocate vulkin objects such as images and buffers.
     */
    VmaAllocator allocator = nullptr;
};
} // namespace vkl_core
#endif