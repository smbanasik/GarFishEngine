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

#include <t_desc_allocgrow.hpp>
#include <t_delstack.hpp>
#include <vkl_images.hpp>

namespace gf {

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

/**
 * @struct DrawContext
 * @brief Bundle for different surface types.
 */
struct DrawContext {
    std::vector<RenderObject> opaque_surfaces;
    std::vector<RenderObject> transparent_surfaces;
    std::vector<RenderObject> static_surfaces; // Meant for UI elements, shouldn't affected by render proj matrix. No culling.
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
