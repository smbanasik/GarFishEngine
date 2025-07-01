/**
* @file
* @brief Contains lower level data types used for Vulkan
* @author Spencer Banasik
*/
#ifndef VKL_SHADER_TYPES_HPP
#define VKL_SHADER_TYPES_HPP

#include <vulkan/vulkan.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace vkl {

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
