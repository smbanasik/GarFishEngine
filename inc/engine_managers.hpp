// Spencer Banasik
// Created: 12/16/2024
// Last Modified: 12/16/2024
// Description:
// Holds global handles for libraries such as GLFW and Vulkan
#ifndef GF_ENGINE_MANAGERS_HPP
#define GF_ENGINE_MANAGERS_HPP

#include <vector>
#include <string>
#include <stdint.h>

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace GF {

constexpr bool DEBUG_USE_VALIDATION = true;

// The GLFWManager owns all GLFW resources and the library
class GLFWManager {
public:
    GLFWwindow* window = nullptr;
    bool is_init = false;

    GLFWManager(VkExtent2D window_dims, const std::string& title);
    ~GLFWManager();

    GLFWManager& get();

private:
    static GLFWManager* loaded_glfw;
    GLFWManager(const GLFWManager& other) = delete;
    GLFWManager& operator=(const GLFWManager& other) = delete;
    GLFWwindow* init_window(GLFWwindow* window, VkExtent2D window_dims, const std::string& title);
};

// Bundle of data for VkManager
struct SwapChain {
    VkSwapchainKHR swapchain{};
    VkFormat swapchain_format{};
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    VkExtent2D swapchain_extent{};
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
};


}
#endif