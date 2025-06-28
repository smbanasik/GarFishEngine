/**
* @file
* @brief Contains the set of functions relevant for pipelines and shaders
* @author Spencer Banasik
*/
#ifndef VK_PIPELINES_HPP
#define VK_PIPELINES_HPP

#include <vector>

#include <vulkan/vulkan.h>

namespace gf {
namespace vk_pipe {

/**
 * @brief Creates a VKShaderModule from a compiled shader.
 * @param file_path [in] Path to shader module.
 * @param device [in] Device shader is related to.
 * @param shader_module [out] A reference to the shader module which will be modified.
 * @returns Whether the operation was successful or not
 */
bool load_shader_module(const char* file_path, VkDevice device, VkShaderModule* shader_module);

/**
 * @class PipelineBuilder
 * @brief Configures a VkPipeline with method functions.
 * @details As opposed to compute pipelkines, graphics pipelines have many 
 * options that need to be selected, wth multiple info objects needing to 
 * be created and defined before the pipeline is ready.
 */
class PipelineBuilder {
public:
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineColorBlendAttachmentState color_blend_attachment;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineLayout pipeline_layout;
    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    VkPipelineRenderingCreateInfo render_info;
    VkFormat color_attachment_format;

    PipelineBuilder() {
        clear();
    }

    /**
     * @brief Reset parameters to default state.
     */
    void clear();

    /**
     * @brief Build the pipeline with the given parameters.
     * @param [in] device Logical device to create the pipelines
     * @returns A created VkPipeline.
     */
    VkPipeline build_pipeline(VkDevice device);

    PipelineBuilder& set_shaders(VkShaderModule vertex, VkShaderModule frag);
    PipelineBuilder& set_input_topology(VkPrimitiveTopology topology);
    PipelineBuilder& set_polygon_mode(VkPolygonMode mode);
    PipelineBuilder& set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace);
    PipelineBuilder& set_multisampling_none();
    PipelineBuilder& disable_blending();
    PipelineBuilder& enable_blending_additive();
    PipelineBuilder& enable_blending_alphablend();
    PipelineBuilder& set_color_attachment_format(VkFormat format);
    PipelineBuilder& set_depth_format(VkFormat format);
    PipelineBuilder& disable_depthtest();
    PipelineBuilder& enable_depthtest(bool depth_write_enable, VkCompareOp op);
};

}
}
#endif