// Spencer Banasik
// Created: 12/28/2024
// Last Modified: 12/30/2024
// Description:
// Code for vulkan pipelines
#ifndef VK_PIPELINES_HPP
#define VK_PIPELINES_HPP

#include <vector>

#include <vulkan/vulkan.h>

namespace gf {
namespace vk_pipe {

bool load_shader_module(const char* file_path, VkDevice device, VkShaderModule* shader_module);

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

    void clear();
    VkPipeline build_pipeline(VkDevice device);
    PipelineBuilder& set_shaders(VkShaderModule vertex, VkShaderModule frag);
    PipelineBuilder& set_input_topology(VkPrimitiveTopology topology);
    PipelineBuilder& set_polygon_mode(VkPolygonMode mode);
    PipelineBuilder& set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace);
    PipelineBuilder& set_multisampling_none();
    PipelineBuilder& disable_blending();
    PipelineBuilder& set_color_attachment_format(VkFormat format);
    PipelineBuilder& set_depth_format(VkFormat format);
    PipelineBuilder& disable_depthtest();
    PipelineBuilder& enable_depthtest(bool depth_write_enable, VkCompareOp op);
};

}
}
#endif