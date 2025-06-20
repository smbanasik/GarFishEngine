// Spencer Banasik
// Created: 12/18/2024
// Last Modified: 1/15/2025
// Description:
// Data types meant for gf_vulkan
#ifndef VK_TYPES_HPP
#define VK_TYPES_HPP

#include <vector>
#include <stdint.h>
#include <functional>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vk_descriptors.hpp>
#include <engine_types.hpp>
#include <gf_util.hpp>
#include <vk_images.hpp>

namespace gf {
// All data for a given frame bundled together

/**
 * @struct Frame
 * @brief An abstraction containing all of the data to produce a frame in the swapchain.
 * @author Spencer Banasik
 */
struct Frame {

    /**
     * @brief A synchronization variable for GPU -> CPU communication.
     * @details The fence is used by the GPU to tell the CPU that it is finished
     * with the command buffer. We wait for the fence before using the resources associated
     * with it. You'll see this used at the beginning of the render loop.
     */
    VkFence render_fence{};

    /**
     * @brief Synchronization variables for GPU -> GPU syncrhonization.
     * @details The swampchain_semaphore and render_semaphore are used to 
     * organize the order that GPU commands are run. In our case,
     * swapchain_semaphore is used to have the render commands wait for the 
     * swapchain, and the render_semaphore will control when the image 
     * is presented for the OS. These are set at the end of the render loop.
     */
    VkSemaphore swapchain_semaphore{}, render_semaphore{};

    /**
     * @brief Allocator for a command buffer.
     * @details A pool can have multiple buffers allocated to it,
     * but we can only record commands from one thread at a time,
     * so we'll pair the allocators and buffers together.
     * @note One use for having multiple buffers with a pool is if you
     * wanted to store and reuse buffers.
     */
    VkCommandPool command_pool{};

    /**
     * @brief Command buffer to hold draw commands.
     * @details Command buffers begin recording by calling `vkBeginCommandBuffer()`,
     * allowing it to receive drawing commands. Once done, call `vkEndCommandBuffer()` 
     * and it is ready for execution.
     * `vkQueueSubmit()` submits the buffer to a queue and is pending. Once the GPU is done
     * `vkResetCommandBuffer()` can be called.
     */
    VkCommandBuffer command_buffer{};

    /**
     * @brief Holds functions to execute.
     * @details Descriptors accumulate pipelines, shaders, and other items that have
     * manual deletion functions. Unless/until these get RAII, this is used to handle their
     * lifetimes and resource cleanup.
     */
    DeletionStack deletion_stack;

    /**
     * @brief An allocator for descriptors
     * @todo implement documentation
     */
    vk_desc::DescriptorAllocatorGrowable frame_descriptors;
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
    vk_img::AllocatedBuffer vertex_buffer;
    vk_img::AllocatedBuffer index_buffer;
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
