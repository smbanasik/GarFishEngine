#include <engine_managers.hpp>

#include <vector>
#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>

GF::GLFWManager::GLFWManager(VkExtent2D window_dims, const std::string& title) {

    if (!glfwInit()) {
        std::cout << "| ERROR: glfw failed to init. Aborting";
        return;
    }

    if ((window = init_window(window, window_dims, title)) == nullptr) {
        std::cout << "| ERROR: Window wasn't initialized! Aborting...";
        return;
    }

    is_init = true;

}
GF::GLFWManager::~GLFWManager() {
    glfwDestroyWindow(window);
    window = nullptr;

    glfwTerminate();
}

GLFWwindow* GF::GLFWManager::init_window(GLFWwindow* window, VkExtent2D window_dims, const std::string& title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: change this later
    window = glfwCreateWindow(window_dims.width, window_dims.height, title.c_str(), nullptr, nullptr);

    return window;
}

GF::VkManager::VkManager(GLFWwindow* window, uint32_t width, uint32_t height) {

    init_vulkan(window);
    create_swapchain(width, height);

    is_init = true;

}
GF::VkManager::~VkManager() {
    
    destroy_swapchain();

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);

    vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    vkDestroyInstance(instance, nullptr);
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
    vkb::PhysicalDevice phys_device = selector.set_minimum_version(1, 3)
        .set_required_features_13(features13)
        .set_required_features_12(features12)
        .set_surface(surface)
        .select()
        .value();

    vkb::DeviceBuilder device_builder{ phys_device };
    vkb::Device device = device_builder.build().value();

    this->device = device.device;
    gpu = phys_device.physical_device;
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