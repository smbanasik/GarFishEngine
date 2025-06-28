// Spencer Banasik
// Created: 1/6/2025
// Last Modified: 1/6/2025
#include <vk_core.hpp>

#include <utility>

#include <vulkan/vulkan.h>
#include <VkBootstrap.h>

#include <gl_manager.hpp>

static constexpr bool DEBUG_USE_VALIDATION = true; // TODO: move this to options

gf::vk_core::VKCore::VKCore(gl::GLManager* gl_manager, gl::WInputContext* gl_context) {
    vkb::InstanceBuilder builder;

    auto inst = builder.set_app_name("GarFishEngine")
        .request_validation_layers(DEBUG_USE_VALIDATION)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();

    vkb::Instance vkb_inst = inst.value();

    instance = vkb_inst.instance;
    debug_messenger = vkb_inst.debug_messenger;

    gl_manager->create_surface(gl_context, this);

    VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    features13.dynamicRendering = true;
    features13.synchronization2 = true;

    VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    vkb::PhysicalDevice vkb_phys_device = selector.set_minimum_version(1, 3)
        .set_required_features_13(features13)
        .set_required_features_12(features12)
        .set_surface(surface)
        .select()
        .value();

    vkb::DeviceBuilder device_builder{ vkb_phys_device };
    vkb::Device vkb_device = device_builder.build().value();

    device = vkb_device.device;
    gpu = vkb_phys_device.physical_device;

    graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

}
gf::vk_core::VKCore::~VKCore() {

    vkDeviceWaitIdle(device);
    if (instance != nullptr) {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyDevice(device, nullptr);
        vkb::destroy_debug_utils_messenger(instance, debug_messenger);
        vkDestroyInstance(instance, nullptr);
    }
}
gf::vk_core::VKCore::VKCore(VKCore&& other) noexcept 
    : instance(std::move(other.instance)),
    gpu (std::move(other.gpu)),
    device (std::move(other.device)),
    surface (std::move(other.surface)),
    debug_messenger (std::move(other.debug_messenger)),
    graphics_queue (std::move(other.graphics_queue)),
    graphics_queue_family (std::move(other.graphics_queue_family)) {
    other.instance = nullptr;
    other.gpu = nullptr;
    other.device = nullptr;
    other.surface = nullptr;
    other.debug_messenger = nullptr;
    other.graphics_queue = nullptr;
    other.graphics_queue_family = 0;
}