// Spencer Banasik
// Created: 1/6/2025
// Last Modified: 1/6/2025
// Description:
// Core structure of vulkan, owns the instance, logical device,
// and surface
#ifndef VK_CORE_HPP
#define VK_CORE_HPP

#include <vulkan/vulkan.h>

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

    VkInstance instance = nullptr;
    VkPhysicalDevice gpu = nullptr;
    VkDevice device = nullptr;
    VkSurfaceKHR surface = nullptr;
    VkDebugUtilsMessengerEXT debug_messenger = nullptr;
    VkQueue graphics_queue = nullptr;
    uint32_t graphics_queue_family = 0;
};
}
}
#endif
