#include <mat_metrough.hpp>

#include <iostream>

#include <gf_vulkan.hpp>
#include <vkl_initializers.hpp>

vkh_mat::GLTFMetallic_Roughness::GLTFMetallic_Roughness()
    : opaque_pipeline(),
    transparent_pipeline(),
    material_layout(),
    writer(),
    device() {};
vkh_mat::GLTFMetallic_Roughness::GLTFMetallic_Roughness(VkDevice* device)
    : opaque_pipeline(),
    transparent_pipeline(),
    material_layout(),
    writer(),
    device(device) {};
vkh_mat::GLTFMetallic_Roughness::GLTFMetallic_Roughness(gf::VkManager* engine)
    : opaque_pipeline(),
    transparent_pipeline(),
    material_layout(),
    writer(),
    device(&engine->core.device) {
    build_pipelines(engine);
}
vkh_mat::GLTFMetallic_Roughness::GLTFMetallic_Roughness(GLTFMetallic_Roughness&& other) noexcept
    : opaque_pipeline(std::move(other.opaque_pipeline)),
    transparent_pipeline(std::move(other.transparent_pipeline)),
    material_layout(std::move(other.material_layout)),
    writer(std::move(other.writer)),
    device(std::move(other.device)) {
    other.device = nullptr;
}
vkh_mat::GLTFMetallic_Roughness::~GLTFMetallic_Roughness() {
    if (device != nullptr)
        clear_resources(*device);
}

void vkh_mat::GLTFMetallic_Roughness::build_pipelines(gf::VkManager* engine) {
    VkShaderModule mesh_vertex_shader;
    if (!gf::vk_pipe::load_shader_module("../../shaders/mesh.vert.spv", engine->core.device, &mesh_vertex_shader))
        std::cout << "| ERROR: vertex shader not built.\n";
    VkShaderModule mesh_frag_shader;
    if (!gf::vk_pipe::load_shader_module("../../shaders/mesh.frag.spv", engine->core.device, &mesh_frag_shader))
        std::cout << "| ERROR: fragment shader not built.\n";

    VkPushConstantRange matrix_range{};
    matrix_range.offset = 0;
    matrix_range.size = sizeof(gf::GPUDrawPushConstants);
    matrix_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    gf::vk_desc::DescriptorLayoutBuilder layout_builder;
    layout_builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    layout_builder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    layout_builder.add_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    material_layout = layout_builder.build(engine->core.device,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    VkDescriptorSetLayout layouts[] = { engine->gpu_scene_data_descriptor_layout,
        material_layout };

    VkPipelineLayoutCreateInfo mesh_layout_info = gf::vk_init::pipeline_layout_info();
    mesh_layout_info.setLayoutCount = 2;
    mesh_layout_info.pSetLayouts = layouts;
    mesh_layout_info.pPushConstantRanges = &matrix_range;
    mesh_layout_info.pushConstantRangeCount = 1;

    VkPipelineLayout new_layout;
    vkCreatePipelineLayout(engine->core.device, &mesh_layout_info, nullptr, &new_layout);

    // This is a SOFT COPY! Meaning that we can only destroy one of them!
    opaque_pipeline.layout = new_layout;
    transparent_pipeline.layout = new_layout;

    gf::vk_pipe::PipelineBuilder pipe_builder;
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

    opaque_pipeline.pipeline = pipe_builder.build_pipeline(engine->core.device);

    pipe_builder.enable_blending_additive();
    pipe_builder.enable_depthtest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);

    transparent_pipeline.pipeline = pipe_builder.build_pipeline(engine->core.device);

    vkDestroyShaderModule(engine->core.device, mesh_frag_shader, nullptr);
    vkDestroyShaderModule(engine->core.device, mesh_vertex_shader, nullptr);
}
void vkh_mat::GLTFMetallic_Roughness::clear_resources(VkDevice device) {
    vkDestroyPipelineLayout(device, transparent_pipeline.layout, nullptr);
    vkDestroyDescriptorSetLayout(device, material_layout, nullptr);

    vkDestroyPipeline(device, opaque_pipeline.pipeline, nullptr);
    vkDestroyPipeline(device, transparent_pipeline.pipeline, nullptr);
}

gf::MaterialInstance vkh_mat::GLTFMetallic_Roughness::write_material(VkDevice device, gf::MaterialPass pass,
    const IMaterialResources& resources, gf::vk_desc::DescriptorAllocatorGrowable& descriptor_allocator) {

    const MaterialResources* mat_resources = dynamic_cast<const MaterialResources*>(&resources);

    gf::MaterialInstance mat_data;
    mat_data.pass_type = pass;
    if (pass == gf::MaterialPass::Transparent)
        mat_data.pipeline = &transparent_pipeline;
    else
        mat_data.pipeline = &opaque_pipeline;

    mat_data.material_set = descriptor_allocator.allocate(device, material_layout);
    writer.clear();
    writer.write_buffer(0, mat_resources->data_buffer, sizeof(MaterialConstants),
        mat_resources->data_buffer_offset, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.write_image(1, mat_resources->color_image.image_view, mat_resources->color_sampler,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    writer.write_image(2, mat_resources->metal_rough_image.image_view, mat_resources->metal_rough_sampler,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    writer.update_set(device, mat_data.material_set);
    return mat_data;
}