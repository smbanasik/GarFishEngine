/**
* @file
* @brief File that contains the VKCore class.
* @author Spencer Banasik
* @date Created: 1/6/2025
* @date Last Modified: 6/16/2025
*/
#ifndef VKL_CORE_HPP
#define VKL_CORE_HPP

#include <utility>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace gf {
namespace wi {
class WIManager;
class WInputContext;
}
namespace vk_core {
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
    VKCore(wi::WIManager* wi_manager, wi::WInputContext* wi_context);
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

/**
* @class Alloc
* @brief Logical abstraction of vma allocator with RAII.
* @author Spencer Banasik
* @details Contains a vma allocator with the rule of 5.
* Copying is not permitted, but has move operations available.
*/
class Alloc {
public:
    
    /**
    * @brief Construct the allocator with information from VKCore.
    * @author Spencer Banasik
    */
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
    
    /**
     * @brief Allocator from VMA
     * @details VMA is an easy way to avoid creating our own method of
     * allocating objects in vulkan, which can be very error prone.
     * This is used to allocate vulakn objects such as images and buffers.
     */
    VmaAllocator allocator = nullptr;
};


}
}
#endif
