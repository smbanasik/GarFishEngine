// Spencer Banasik
// Created: 1/6/2025
// Last Modified: 1/6/2025
// Description:
// Core structure of vulkan, owns the instance, logical device,
// and surface
#ifndef VK_CORE_HPP
#define VK_CORE_HPP

#include <utility>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace gf {
namespace gl {
class GLManager;
class WInputContext;
}
namespace vk_core {
class VKCore {
public:

    VKCore(gl::GLManager* gl_manager, gl::WInputContext* gl_context);
    ~VKCore();
    VKCore(VKCore&& other) noexcept;
    VKCore& operator=(VKCore&& other) noexcept {
        instance = std::move(other.instance);
        gpu = std::move(other.gpu);
        device = std::move(other.device);
        surface = std::move(other.surface);
        debug_messenger = std::move(other.debug_messenger);
        graphics_queue = std::move(other.graphics_queue);
        graphics_queue_family = std::move(other.graphics_queue_family);
        other.instance = nullptr;
        other.gpu = nullptr;
        other.device = nullptr;
        other.surface = nullptr;
        other.debug_messenger = nullptr;
        other.graphics_queue = nullptr;
        other.graphics_queue_family = 0;
    }
    VKCore(VKCore& other) = delete;
    VKCore& operator=(VKCore& other) = delete;

    VkInstance instance = nullptr;
    VkPhysicalDevice gpu = nullptr;
    VkDevice device = nullptr;
    VkSurfaceKHR surface = nullptr;
    VkDebugUtilsMessengerEXT debug_messenger = nullptr;
    VkQueue graphics_queue = nullptr;
    uint32_t graphics_queue_family = 0;
};
class Alloc {
public:
    
    Alloc(VKCore* core) {
        VmaAllocatorCreateInfo allocator_info = {};
        allocator_info.physicalDevice = core->gpu;
        allocator_info.device = core->device;
        allocator_info.instance = core->instance;
        allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocator_info, &allocator);
    }
    ~Alloc() {
        if(allocator != nullptr)
            vmaDestroyAllocator(allocator);
    }
    Alloc(Alloc&& other) noexcept
    : allocator(std::move(other.allocator)) {
        other.allocator = nullptr;
    }
    Alloc& operator=(Alloc&& other) noexcept {
        allocator = std::move(other.allocator);
        other.allocator = nullptr;
    }
    Alloc(Alloc& other) = delete;
    Alloc& operator=(Alloc& other) = delete;
    
    VmaAllocator allocator = nullptr;


};


}
}
#endif
