/**
* @file
* @brief Contains lower level material types
* @author Spencer Banasik
*/
#ifndef VKL_MAT_TYPES_HPP
#define VKL_MAT_TYPES_HPP

#include <stdint.h>

#include <vulkan/vulkan.h>

namespace vkl {
/**
 * @brief Material properties for pipeline selection.
 */
enum class MaterialPass : uint8_t {
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
}
#endif
