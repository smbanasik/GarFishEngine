// Spencer Banasik
// Created: 12/18/2024
// Last Modified: 12/28/2024
// Description:
// Data types meant for gf_vulkan
#ifndef VK_TYPES_HPP
#define VK_TYPES_HPP

#include <vector>
#include <stdint.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vk_descriptors.hpp>
#include <engine_types.hpp>

namespace gf {
// All swapchain information bundled together
struct SwapChain {
    VkSwapchainKHR swapchain{};
    VkFormat swapchain_format{};
    VkExtent2D swapchain_extent{};
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
};
// All data for a given frame bundled together
struct FrameData {
    VkFence render_fence{};
    VkSemaphore swapchain_semaphore{}, render_semaphore{};

    VkCommandPool command_pool{};
    VkCommandBuffer command_buffer{};

    DeletionStack deletion_stack;
    DescriptorAllocatorGrowable frame_descriptors;
};
// All data for an image bundled together
struct AllocatedImage {
    VkImage image;
    VkImageView image_view;
    VmaAllocation allocation;
    VkExtent3D image_size;
    VkFormat image_format;
};
// Data for a buffer and it's vma info
struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};
// Interleaved to improve performance with alignment (3 vec4s ig)
struct Vertex {
    glm::vec3 position;
    float uv_x;
    glm::vec3 normal;
    float uv_y;
    glm::vec4 color;
};

// Resources for a full mesh
struct GPUMeshBuffers {
    AllocatedBuffer index_buffer;
    AllocatedBuffer vertex_buffer;
    VkDeviceAddress vertex_buffer_address;
};
// Push constants for a mesh
struct GPUDrawPushConstants {
    glm::mat4 world_matrix;
    VkDeviceAddress vertex_buffer;
};
struct GPUSceneData {
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambient_color;
    glm::vec4 sunlight_direction;
    glm::vec4 sunlight_color;
};
}
#endif
