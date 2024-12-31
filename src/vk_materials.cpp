// Spencer Banasik
// Created: 12/31/2024
// Last Modified: 12/31/2024
#include <vk_materials.hpp>

#include <iostream>

#include <gf_vulkan.hpp>
#include <vk_pipelines.hpp>
#include <vk_descriptors.hpp>
#include <vk_initializers.hpp>

void gf::vk_mat::GLTFMetallic_Roughness::build_pipelines(gf::VkManager* engine) {
    VkShaderModule mesh_vertex_shader;
    if (!vk_pipe::load_shader_module("../../shaders/mesh.vert.spv", engine->device, &mesh_vertex_shader))
        std::cout << "| ERROR: vertex shader not built.\n";
    VkShaderModule mesh_frag_shader;
    if (!vk_pipe::load_shader_module("../../shaders/mesh.frag.spv", engine->device, &mesh_frag_shader))
        std::cout << "| ERROR: fragment shader not built.\n";

    VkPushConstantRange matrix_range{};
    matrix_range.offset = 0;
    matrix_range.size = sizeof(GPUDrawPushConstants);
    matrix_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    DescriptorLayoutBuilder layout_builder;
    layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    layout_builder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    layout_builder.add_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    material_layout = layout_builder.build(engine->device,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    VkDescriptorSetLayout layouts[] = { engine->gpu_scene_data_descriptor_layout,
        material_layout };

    VkPipelineLayoutCreateInfo mesh_layout_info = vk_init::pipeline_layout_info();
    mesh_layout_info.setLayoutCount = 2;
    mesh_layout_info.pSetLayouts = layouts;
    mesh_layout_info.pPushConstantRanges = &matrix_range;
    mesh_layout_info.pushConstantRangeCount = 1;

    VkPipelineLayout new_layout;
    vkCreatePipelineLayout(engine->device, &mesh_layout_info, nullptr, &new_layout);
    
    opaque_pipeline.layout = new_layout;
    transparent_pipeline.layout = new_layout;

    vk_pipe::PipelineBuilder pipe_builder;
    pipe_builder.set_shaders(mesh_vertex_shader, mesh_frag_shader)
        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .set_polygon_mode(VK_POLYGON_MODE_FILL)
        .set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
        .set_multisampling_none()
        .disable_blending()
        .enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL)
        .set_color_attachment_format(engine->drawn_image.image_format)
        .set_depth_format(engine->depth_image.image_format);
    pipe_builder.pipeline_layout = new_layout;
    
    opaque_pipeline.pipeline = pipe_builder.build_pipeline(engine->device);

    pipe_builder.enable_blending_additive();
    pipe_builder.enable_depthtest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);

    transparent_pipeline.pipeline = pipe_builder.build_pipeline(engine->device);

    vkDestroyShaderModule(engine->device, mesh_frag_shader, nullptr);
    vkDestroyShaderModule(engine->device, mesh_vertex_shader, nullptr);
}
void gf::vk_mat::GLTFMetallic_Roughness::clear_resources(VkDevice device) {
    vkDestroyPipelineLayout(device, opaque_pipeline.layout, nullptr);
    vkDestroyPipelineLayout(device, transparent_pipeline.layout, nullptr);
    vkDestroyPipeline(device, opaque_pipeline.pipeline, nullptr);
    vkDestroyPipeline(device, transparent_pipeline.pipeline, nullptr);
    vkDestroyDescriptorSetLayout(device, material_layout, nullptr);
}

gf::MaterialInstance gf::vk_mat::GLTFMetallic_Roughness::write_material(VkDevice device, MaterialPass pass,
    const MaterialResources& resources, DescriptorAllocatorGrowable& descriptor_allocator) {

    MaterialInstance mat_data;
    mat_data.pass_type = pass;
    if (pass == MaterialPass::Transparent)
        mat_data.pipeline = &transparent_pipeline;
    else
        mat_data.pipeline = &opaque_pipeline;

    mat_data.material_set = descriptor_allocator.allocate(device, material_layout);
    writer.clear();
    writer.write_buffer(0, resources.data_buffer, sizeof(MaterialConstants),
        resources.data_buffer_offset, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.write_image(1, resources.color_image.image_view, resources.color_sampler,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    writer.write_image(2, resources.metal_rough_image.image_view, resources.metal_rough_sampler,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    writer.update_set(device, mat_data.material_set);
    return mat_data;
}