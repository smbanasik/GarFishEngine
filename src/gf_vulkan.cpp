// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 12/17/2024
#include <gf_vulkan.hpp>

#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <vk_initializers.hpp>

GF::VkManager* GF::VkManager::loaded_vk = nullptr;

GF::VkManager::VkManager(GLFWwindow* window, uint32_t width, uint32_t height) {
    assert(loaded_vk == nullptr);
    loaded_vk = this;

    init_vulkan(window);
    create_swapchain(width, height);
    create_framedata();

    is_init = true;

}
GF::VkManager::~VkManager() {

    destroy_framedata();
    destroy_swapchain();
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);
    vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    vkDestroyInstance(instance, nullptr);
}

GF::VkManager& GF::VkManager::get() {
    return *loaded_vk;
}

void GF::VkManager::init_vulkan(GLFWwindow* window) {
    vkb::InstanceBuilder builder;

    auto inst = builder.set_app_name("GarFishEngine")
        .request_validation_layers(DEBUG_USE_VALIDATION)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();

    vkb::Instance vkb_inst = inst.value();

    instance = vkb_inst.instance;
    debug_messenger = vkb_inst.debug_messenger;

    auto err = glfwCreateWindowSurface(instance, window, NULL, &surface);
    if (err) {
        std::cout << "| ERROR: surface creation failed \n";
        std::cout << err << "\n";
    }

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

void GF::VkManager::create_swapchain(uint32_t width, uint32_t height) {
    vkb::SwapchainBuilder swapchain_builder{ gpu, device, surface };
    swapchain.swapchain_format = VK_FORMAT_B8G8R8A8_UNORM;

    vkb::Swapchain vkb_swapchain = swapchain_builder
        .set_desired_format(VkSurfaceFormatKHR{ .format = swapchain.swapchain_format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(width, height)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

    swapchain.swapchain_extent = vkb_swapchain.extent;
    swapchain.swapchain = vkb_swapchain.swapchain;
    swapchain.swapchain_images = vkb_swapchain.get_images().value();
    swapchain.swapchain_image_views = vkb_swapchain.get_image_views().value();
}

void GF::VkManager::destroy_swapchain() {

    vkDestroySwapchainKHR(device, swapchain.swapchain, nullptr);

    for (auto it = swapchain.swapchain_image_views.begin(); it != swapchain.swapchain_image_views.end(); it++) {
        vkDestroyImageView(device, *it, nullptr);
    }
}

void GF::VkManager::create_framedata() {

    VkCommandPoolCreateInfo pool_info = init_vk_command_pool_info(graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkFenceCreateInfo fence_info = init_vk_fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphore_info = init_vk_semaphore_info();

    for (auto it = active_frames.begin(); it != active_frames.end(); it++) {

        vkCreateCommandPool(device, &pool_info, nullptr, &(*it).command_pool);
        VkCommandBufferAllocateInfo alloc_info = init_vk_command_allocate_info((*it).command_pool);
        vkAllocateCommandBuffers(device, &alloc_info, &(*it).command_buffer);

        vkCreateFence(device, &fence_info, nullptr, &(*it).render_fence);
        vkCreateSemaphore(device, &semaphore_info, nullptr, &(*it).swapchain_semaphore);
        vkCreateSemaphore(device, &semaphore_info, nullptr, &(*it).render_semaphore);
    }

}

void GF::VkManager::destroy_framedata() {
    vkDeviceWaitIdle(device);

    for (auto it = active_frames.begin(); it != active_frames.end(); it++) {
        vkDestroyCommandPool(device, (*it).command_pool, nullptr);
        vkDestroyFence(device, (*it).render_fence, nullptr);
        vkDestroySemaphore(device, (*it).render_semaphore, nullptr);
        vkDestroySemaphore(device, (*it).swapchain_semaphore, nullptr);
    }
}