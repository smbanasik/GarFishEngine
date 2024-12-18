// Spencer Banasik
// Created: 12/16/2024
// Last Modified: 12/17/2024
// Description:
// Handles abstract and low level vulkan concepts
#ifndef GF_VULKAN_HPP
#define GF_VULKAN_HPP

#include <vector>
#include <string>
#include <stdint.h>
#include <array>

#include <vulkan/vulkan.h>
struct GLFWwindow;

namespace gf {
constexpr bool DEBUG_USE_VALIDATION = true;
constexpr uint8_t FRAME_OVERLAP = 2;
// Bundle of data for VkManager
struct SwapChain {
    VkSwapchainKHR swapchain{};
    VkFormat swapchain_format{};
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    VkExtent2D swapchain_extent{};
};

struct FrameData {
    VkCommandPool command_pool{};
    VkCommandBuffer command_buffer{};
    VkFence render_fence{};
    VkSemaphore swapchain_semaphore{}, render_semaphore{};
};

// The VkManager owns all of the abstract vulkan constructs and handles the
// initialization of the vulkan library
class VkManager {
public:

    VkInstance instance;
    VkPhysicalDevice gpu;
    VkDevice device;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkQueue graphics_queue;
    uint32_t graphics_queue_family;
    SwapChain swapchain;
    std::array<FrameData, FRAME_OVERLAP> active_frames;

    bool is_init = false;

    VkManager(GLFWwindow* window, uint32_t width, uint32_t height);
    ~VkManager();

    VkManager& get();

private:
    static VkManager* loaded_vk;
    VkManager(const VkManager& other) = delete;
    VkManager& operator=(const VkManager& other) = delete;
    void init_vulkan(GLFWwindow* window);
    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();
    void create_framedata();
    void destroy_framedata();

};
}

#endif