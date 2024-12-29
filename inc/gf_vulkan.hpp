// Spencer Banasik
// Created: 12/16/2024
// Last Modified: 12/27/2024
// Description:
// Handles abstract and low level vulkan concepts
#ifndef GF_VULKAN_HPP
#define GF_VULKAN_HPP

#include <vector>
#include <string>
#include <stdint.h>
#include <array>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <engine_types.hpp>
#include <vk_types.hpp>
#include <vk_descriptors.hpp>
#include <vk_pipelines.hpp>

struct GLFWwindow;

namespace gf {
constexpr bool DEBUG_USE_VALIDATION = true;
constexpr uint8_t FRAME_OVERLAP = 2;

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
    VmaAllocator allocator;
    SwapChain swapchain;
    std::array<FrameData, FRAME_OVERLAP> active_frames;
    DescriptorAllocator global_descriptor_allocator;
    VkPipeline gradient_pipeline;
    VkPipelineLayout gradient_pipeline_layout;
    
    AllocatedImage drawn_image;
    VkExtent2D drawn_size;
    VkDescriptorSet drawn_image_descriptors;
    VkDescriptorSetLayout drawn_image_descriptor_layout;
    
    std::vector<ComputeEffect> background_effects; // For fun!
    int current_background_effect{ 0 }; // For fun!

    bool is_init = false;

    VkManager(GLFWwindow* window, uint32_t width, uint32_t height);
    ~VkManager();

    VkManager& get();

    void draw_background(VkCommandBuffer cmd, VkClearColorValue& clear);

private:
    static VkManager* loaded_vk;
    DeletionStack global_deletion_stack;

    VkManager(const VkManager& other) = delete;
    VkManager& operator=(const VkManager& other) = delete;
    void init_vulkan(GLFWwindow* window);
    void create_swapchain(uint32_t width, uint32_t height);
    void create_framedata();
    void create_allocator();
    void init_descriptors();
    void init_pipelines();
    void init_background_pipelines();
    void init_imgui(GLFWwindow* window);

};
}

#endif