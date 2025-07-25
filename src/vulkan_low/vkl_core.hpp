/**
* @file
* @brief File that contains the VKCore class.
* @author Spencer Banasik
*/
#ifndef VKL_CORE_HPP
#define VKL_CORE_HPP

#include <utility>

#include <vulkan/vulkan.h>

namespace gf {
namespace wi {
class WIManager;
class WInputContext;
}
}
namespace vkl_core {
/**
* @class VKCore
* @brief Logical abstraction of many core vulkan objects with RAII.
* @author Spencer Banasik
* @details Vulkan has a lot of boilerplate setup. This includes an instance, 
* a physical device, a logical device, graphics queues, the families, and so on.
* By nature of being a C library, these all have functions for setup and teardown.
* This class wraps all of that into RAII.
* @pre Requires some specific graphics card features to be available.
* @todo Need to consider the speciifcs of this pre condition.
*/
class VKCore {
public:

    /**
    * @brief Constructos a VKCore with a WInputContext and GLManager.
    * @author Spencer Banasik
    * @details Utilizes vkb to quickly assemble a Vulkan instance and debug messenger.
    * Then uses the gl_manager and the context to provide a surface to the VKCore's instance.
    * Once done, queries the graphics card and configures engine features.
    * Then provides physical and logical devices (gpu and device, respectively).
    * Finally, gets the graphics queue and graphics queue family.
    */
    VKCore(gf::wi::WIManager* wi_manager, gf::wi::WInputContext* wi_context);
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

    /**
     * @brief Vulkan API context.
     * @details Handles instance extensions, logging, validation layers, and so on.
     * This is the substitute for global state, since Vulkan has none.
     */
    VkInstance instance = nullptr;

    /**
     * @brief A representation of a physical GPU.
     * @details With the VKInstance, we can query the system for GPUs.
     * The GPU and it's physical capabilities are present here
     */
    VkPhysicalDevice gpu = nullptr;

    /**
     * @brief The driver of the GPU.
     * @details Logical representation of the GPU, specifically it's driver.
     * Created with a list of extensions we're enabling, this is how we actually
     * communicate with the GPU.
     */
    VkDevice device = nullptr;

    /**
     * @brief Window surface that Vulkan renders to.
     */
    VkSurfaceKHR surface = nullptr;

    
    VkDebugUtilsMessengerEXT debug_messenger = nullptr;

    /**
     * @brief Entrypoint for execution commands on GPU.
     */
    VkQueue graphics_queue = nullptr;
    
    
    uint32_t graphics_queue_family = 0;
};

}
#endif
