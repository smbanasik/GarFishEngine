// Spencer Banasik
// Created: 12/18/2024
// Last Modified: 1/7/2025
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
#include <gf_util.hpp>

namespace gf {
// All data for a given frame bundled together
struct Frame {
    VkFence render_fence{};
    VkSemaphore swapchain_semaphore{}, render_semaphore{};

    VkCommandPool command_pool{};
    VkCommandBuffer command_buffer{};

    DeletionStack deletion_stack;
    vk_desc::DescriptorAllocatorGrowable frame_descriptors;
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
// Cool higher level stuff!
enum class MaterialPass :uint8_t {
    MainColor,
    Transparent,
    Other
};
struct MaterialPipeline {
    VkPipeline pipeline;
    VkPipelineLayout layout;
};

struct MaterialInstance {
    MaterialPipeline* pipeline;
    VkDescriptorSet material_set;
    MaterialPass pass_type;
};
struct Bounds {
    glm::vec3 origin;
    float sphereRadius;
    glm::vec3 extents;
};
struct RenderObject {
    uint32_t index_count;
    uint32_t first_index;
    VkBuffer index_buffer;

    MaterialInstance* material;
    Bounds bounds;
    glm::mat4 transform;
    VkDeviceAddress vertex_buffer_address;
};

struct ComputePushConstants {
    glm::vec4 data1;
    glm::vec4 data2;
    glm::vec4 data3;
    glm::vec4 data4;
};
// For fun!
struct ComputeEffect {
    const char* name;
    VkPipeline pipeline;
    VkPipelineLayout layout;
    ComputePushConstants data;
};
}
#endif
