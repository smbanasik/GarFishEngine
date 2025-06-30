/**
* @file
* @brief Contains lower level data types used for Vulkan
* @author Spencer Banasik
*/
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

#include <vkl_descriptors.hpp>
#include <com_delstack.hpp>
#include <vkl_images.hpp>

namespace gf {
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
    com::DeletionStack deletion_stack;

    /**
     * @brief An allocator for descriptors
     * @todo implement documentation
     */
    vk_desc::DescriptorAllocatorGrowable frame_descriptors;
};

/**
 * @brief A vertex and it's associated data
 * @details A vertex has a position, a normal, texture coordinates, and 
 * a color. The texture coordinates are interleaved for packing purposes.
 * Note that texture and color is prorated between vertices.
 */
struct Vertex {
    glm::vec3 position;
    float uv_x;
    glm::vec3 normal;
    float uv_y;
    glm::vec4 color;
};

/**
 * @brief Resources for a mesh
 * @details This ultimately starts in the shader, where we
 * send mesh data via a push constant with an address pointing to an SSBO. Due to this,
 * we want the buffer address specifically, and we may as well store it here.
 * The vertex buffer contains the actual data and the index buffer is used to
 * order the data.
 */
struct GPUMeshBuffers {
    vk_img::AllocatedBuffer vertex_buffer;
    vk_img::AllocatedBuffer index_buffer;
    VkDeviceAddress vertex_buffer_address;
};
/**
 * @brief Push constants for shader
 * @details Unlike GPUMeshBuffers, this represents the actual push constants
 * used in the shader. The world matrix is used to shift the vertices from model space
 * to world space, and the VkDeviceAddress is linked to the same one in the GPUMeshBuffers.
 */
struct GPUDrawPushConstants {
    glm::mat4 world_matrix;
    VkDeviceAddress vertex_buffer;
};
/**
 * @brief Global data for shaders
 */
struct GPUSceneData {
    /**
     * @brief View matrix for camera
     */
    glm::mat4 view;
    /**
     * @brief Projection matrix for camera
     */
    glm::mat4 proj;
    glm::mat4 viewproj;
    glm::vec4 ambient_color;
    glm::vec4 sunlight_direction;
    glm::vec4 sunlight_color;
};
/**
 * @brief Material properties for pipeline selection.
 */
enum class MaterialPass :uint8_t {
    MainColor,
    Transparent,
    Other
};
/**
 * @brief Holds a pipeline and it's layout
 */
struct MaterialPipeline {
    VkPipeline pipeline;
    VkPipelineLayout layout;
};
/**
 * @brief The data required to create a material
 * @details Contains a pointer to a pipeline (held by a base material),
 * a descriptor set for providing resources to the shader (like textures),
 * and a pass type.
 */
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
/**
 * @brief A flat object containing everything needed to render an object.
 * @details This is used directly in the draw loop.
 * @todo make this better
 */
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
