// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 12/29/2024
#include <gf_vulkan.hpp>

#include <vector>
#include <iostream>
#include <cmath>
#include <cstring>

#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <vk_initializers.hpp>
#include <engine_types.hpp>
#include <vk_types.hpp>
#include <vk_descriptors.hpp>
#include <vk_images.hpp>
#include <gl_manager.hpp>
// TEMPORARY
#include <vulkan/vk_enum_string_helper.h>
#define VK_CHECK(x)                                                     \
    do {                                                                \
        VkResult err = x;                                               \
        if (err) {                                                      \
            std::cout << "Detected Vulkan error: " << string_VkResult(err) << "\n"; \
            abort();                                                    \
        }                                                               \
    } while (0)

gf::VkManager* gf::VkManager::loaded_vk = nullptr;

gf::VkManager::VkManager(gl::GLManager & gl_manager, gl::WInputContext & gl_context)
    : core(&gl_manager, &gl_context), alloc(&core), 
    swapchain(&core, gl_context.window.get_window_dims().width, gl_context.window.get_window_dims().height),
    frame_data(&core), imm_frame(&core) {
    
    assert(loaded_vk == nullptr);
    loaded_vk = this;

    init_swapchain(gl_context.window.get_window_dims().width, gl_context.window.get_window_dims().height);
    init_descriptors();
    init_pipelines();
    init_imgui(gl_manager.get_window(&gl_context));
    init_default_data();

    is_init = true;

}
gf::VkManager::~VkManager() {

    vkDeviceWaitIdle(core.device);

    metal_rough_material.clear_resources(core.device);

    global_deletion_stack.flush();
}

gf::VkManager& gf::VkManager::get() {
    return *loaded_vk;
}

void gf::VkManager::draw_background(VkCommandBuffer cmd, VkClearColorValue& clear) {
    VkImageSubresourceRange clear_range = vk_init::subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
    //vkCmdClearColorImage(cmd, drawn_image.image, VK_IMAGE_LAYOUT_GENERAL, &clear, 1, &clear_range);

    ComputeEffect& effect = background_effects[current_background_effect];
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, gradient_pipeline_layout, 0, 1, &drawn_image_descriptors, 0, nullptr);

    vkCmdPushConstants(cmd, gradient_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &effect.data);
    vkCmdDispatch(cmd, std::ceil(drawn_size.width / 16.0), std::ceil(drawn_size.width / 16.0), 1);
}

void gf::VkManager::draw_geometry(VkCommandBuffer cmd, Frame* frame) {
    
    AllocatedBuffer gpu_scene_data_buffer = create_buffer(sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    frame->deletion_stack.push_function([gpu_scene_data_buffer, this] {
        destroy_buffer(gpu_scene_data_buffer);
        });
    GPUSceneData* scene_uniform_data = reinterpret_cast<GPUSceneData*>(gpu_scene_data_buffer.allocation->GetMappedData());
    *scene_uniform_data = scene_data;
    
    VkDescriptorSet global_descriptor = frame->frame_descriptors.allocate(core.device, gpu_scene_data_descriptor_layout);
    DescriptorWriter writer;
    writer.write_buffer(0, gpu_scene_data_buffer.buffer, sizeof(GPUSceneData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.update_set(core.device, global_descriptor);

    VkRenderingAttachmentInfo color_attachment = vk_init::attachment_info(drawn_image.image_view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingAttachmentInfo depth_attachment = vk_init::depth_attachment_info(depth_image.image_view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    VkRenderingInfo render_info = vk_init::rendering_info(drawn_size, &color_attachment, &depth_attachment);
    vkCmdBeginRendering(cmd, &render_info);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, triangle_pipeline.pipeline);
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = drawn_size.width;
    viewport.height = drawn_size.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = drawn_size.width;
    scissor.extent.height = drawn_size.height;
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    vkCmdDraw(cmd, 3, 1, 0, 0);

    for (const RenderObject& draw : main_draw_context.opaque_surfaces) {

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->pipeline);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->layout, 0, 1, &global_descriptor, 0, nullptr);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, draw.material->pipeline->layout, 1, 1, &draw.material->material_set, 0, nullptr);
        vkCmdBindIndexBuffer(cmd, draw.index_buffer, 0, VK_INDEX_TYPE_UINT32);
        GPUDrawPushConstants push_constants;
        push_constants.vertex_buffer = draw.vertex_buffer_address;
        push_constants.world_matrix = draw.transform;
        vkCmdPushConstants(cmd, draw.material->pipeline->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);
        vkCmdDrawIndexed(cmd, draw.index_count, 1, draw.first_index, 0, 0);
    }
    vkCmdEndRendering(cmd);
}
void gf::VkManager::update_scene(uint32_t width, uint32_t height) {
    camera.update();

    main_draw_context.opaque_surfaces.clear();
    loaded_nodes["Suzanne"]->draw(glm::mat4{ 1.f }, main_draw_context);

    for (int i = 0; i < 3; i++) {
        glm::mat4 scale = glm::scale(glm::mat4{ 1.f }, glm::vec3{ 0.2 });
        glm::mat4 translation = glm::translate(glm::mat4{ 1.f }, glm::vec3{ i, 1, 0 });

        loaded_nodes["Cube"]->draw(translation * scale, main_draw_context);
    }

    scene_data.view = camera.get_view_matrix();
    scene_data.proj = glm::perspective(glm::radians(70.f), static_cast<float>(width) / static_cast<float>(height), 10000.f, 0.1f);
    scene_data.proj[1][1] *= -1;
    scene_data.viewproj = scene_data.proj * scene_data.view;

    scene_data.ambient_color = glm::vec4(.1f);
    scene_data.sunlight_color = glm::vec4(1.f);
    scene_data.sunlight_direction = glm::vec4(0, 1, 0.5, 1.f);
}

void gf::VkManager::init_swapchain(uint32_t width, uint32_t height) {
    
    //create_swapchain(width, height);

    VkExtent3D image_size = { width, height, 1 };
    drawn_image.image_format = VK_FORMAT_R16G16B16A16_SFLOAT;
    drawn_image.image_size = image_size;
    VkImageUsageFlags drawn_image_usage{};
    drawn_image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawn_image_usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawn_image_usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawn_image_usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VmaAllocationCreateInfo image_alloc_info = {};
    image_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    image_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkImageCreateInfo image_info = vk_init::image_info(drawn_image.image_format, image_size, drawn_image_usage);
    vmaCreateImage(alloc.allocator, &image_info, &image_alloc_info, &drawn_image.image, &drawn_image.allocation, nullptr);
    VkImageViewCreateInfo view_info = vk_init::image_view_info(drawn_image.image_format, drawn_image.image, VK_IMAGE_ASPECT_COLOR_BIT);
    vkCreateImageView(core.device, &view_info, nullptr, &drawn_image.image_view);

    depth_image.image_format = VK_FORMAT_D32_SFLOAT;
    depth_image.image_size = image_size;
    VkImageUsageFlags depth_usage_flags{};
    depth_usage_flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VkImageCreateInfo depth_image_info = vk_init::image_info(depth_image.image_format, image_size, depth_usage_flags);
    vmaCreateImage(alloc.allocator, &depth_image_info, &image_alloc_info, &depth_image.image, &depth_image.allocation, nullptr);
    VkImageViewCreateInfo depth_view_info = vk_init::image_view_info(depth_image.image_format, depth_image.image, VK_IMAGE_ASPECT_DEPTH_BIT);
    vkCreateImageView(core.device, &depth_view_info, nullptr, &depth_image.image_view);
    global_deletion_stack.push_function([this]() {
        //destroy_swapchain();

        vkDestroyImageView(this->core.device, this->drawn_image.image_view, nullptr);
        vmaDestroyImage(this->alloc.allocator, this->drawn_image.image, this->drawn_image.allocation);
        vkDestroyImageView(this->core.device, this->depth_image.image_view, nullptr);
        vmaDestroyImage(this->alloc.allocator, this->depth_image.image, this->depth_image.allocation);
        });

}

void gf::VkManager::init_descriptors() {

    std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = {
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}
    };

    global_descriptor_allocator.init(core.device, 10, sizes);
    
    {
        DescriptorLayoutBuilder builder;
        builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        drawn_image_descriptor_layout = builder.build(core.device, VK_SHADER_STAGE_COMPUTE_BIT);
    }
    drawn_image_descriptors = global_descriptor_allocator.allocate(core.device, drawn_image_descriptor_layout);
    {
        DescriptorLayoutBuilder builder;
        builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        gpu_scene_data_descriptor_layout = builder.build(core.device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    }
    {
        DescriptorLayoutBuilder builder;
        builder.add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        single_image_descriptor_layout = builder.build(core.device, VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    DescriptorWriter writer;
    writer.write_image(0, drawn_image.image_view, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    writer.update_set(core.device, drawn_image_descriptors);

    global_deletion_stack.push_function([this]() {
        global_descriptor_allocator.destroy_pools(core.device);
        vkDestroyDescriptorSetLayout(core.device, drawn_image_descriptor_layout, nullptr);
        vkDestroyDescriptorSetLayout(core.device, gpu_scene_data_descriptor_layout, nullptr);
        vkDestroyDescriptorSetLayout(core.device, single_image_descriptor_layout, nullptr);
        });
}

void gf::VkManager::init_pipelines() {
    init_background_pipelines();
    init_triangle_pipeline();
    init_mesh_pipeline();
    metal_rough_material.build_pipelines(this);
}
void gf::VkManager::init_background_pipelines() {

    VkPipelineLayoutCreateInfo compute_layout{};
    compute_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    compute_layout.pNext = nullptr;
    compute_layout.pSetLayouts = &drawn_image_descriptor_layout;
    compute_layout.setLayoutCount = 1;

    VkPushConstantRange push_constant{};
    push_constant.offset = 0;
    push_constant.size = sizeof(ComputePushConstants);
    push_constant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    compute_layout.pPushConstantRanges = &push_constant;
    compute_layout.pushConstantRangeCount = 1;

    vkCreatePipelineLayout(core.device, &compute_layout, nullptr, &gradient_pipeline_layout);

    VkShaderModule gradient_shader;
    if (!vk_pipe::load_shader_module("../../shaders/gradient_color.comp.spv", core.device, &gradient_shader))
        std::cout << "| ERROR: compute shader was not built.\n";
    VkShaderModule sky_shader;
    if (!vk_pipe::load_shader_module("../../shaders/gradient_color.comp.spv", core.device, &sky_shader)) // TEMP CHANGED TO GRADIENT
        std::cout << "| ERROR: compute shader was not built.\n";

    VkPipelineShaderStageCreateInfo stage_info{};
    stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_info.pNext = nullptr;
    stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage_info.module = gradient_shader;
    stage_info.pName = "main";

    VkComputePipelineCreateInfo compute_create_info{};
    compute_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    compute_create_info.pNext = nullptr;
    compute_create_info.layout = gradient_pipeline_layout;
    compute_create_info.stage = stage_info;

    ComputeEffect gradient;
    gradient.layout = gradient_pipeline_layout;
    gradient.name = "gradient";
    gradient.data = {};
    gradient.data.data1 = { 1, 0, 0, 1 };
    gradient.data.data2 = { 0, 0.5, 1, 1 };

    vkCreateComputePipelines(core.device, VK_NULL_HANDLE, 1, &compute_create_info, nullptr, &gradient.pipeline);

    compute_create_info.stage.module = sky_shader;
    ComputeEffect sky;
    sky.layout = gradient_pipeline_layout;
    sky.name = "sky";
    sky.data = {};
    sky.data.data1 = { 0.1, 0.2, 0.4, 0.97 };

    vkCreateComputePipelines(core.device, VK_NULL_HANDLE, 1, &compute_create_info, nullptr, &sky.pipeline);

    background_effects.push_back(gradient);
    background_effects.push_back(sky);

    vkDestroyShaderModule(core.device, gradient_shader, nullptr);
    vkDestroyShaderModule(core.device, sky_shader, nullptr);

    global_deletion_stack.push_function([=]() {
        vkDestroyPipelineLayout(core.device, gradient_pipeline_layout, nullptr);
        vkDestroyPipeline(core.device, gradient.pipeline, nullptr);
        vkDestroyPipeline(core.device, sky.pipeline, nullptr);
        });
}

void gf::VkManager::init_triangle_pipeline() {
    VkShaderModule triangle_frag_shader;
    if (!vk_pipe::load_shader_module("../../shaders/colored_tri.frag.spv", core.device, &triangle_frag_shader))
        std::cout << "Error when building the triangle fragment shader module\n";

    VkShaderModule triangle_vertex_shader;
    if (!vk_pipe::load_shader_module("../../shaders/colored_tri.vert.spv", core.device, &triangle_vertex_shader))
        std::cout << "Error when building the triangle vertex shader module\n";

    VkPipelineLayoutCreateInfo pipeline_layout_info = vk_init::pipeline_layout_info();
    vkCreatePipelineLayout(core.device, &pipeline_layout_info, nullptr, &triangle_pipeline.layout);

    vk_pipe::PipelineBuilder pipe_builder;

    pipe_builder.set_shaders(triangle_vertex_shader, triangle_frag_shader)
        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .set_polygon_mode(VK_POLYGON_MODE_FILL)
        .set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
        .set_multisampling_none()
        .disable_blending()
        .disable_depthtest()
        .set_color_attachment_format(drawn_image.image_format)
        .set_depth_format(depth_image.image_format);
    pipe_builder.pipeline_layout = triangle_pipeline.layout;

    triangle_pipeline.pipeline = pipe_builder.build_pipeline(core.device);

    vkDestroyShaderModule(core.device, triangle_frag_shader, nullptr);
    vkDestroyShaderModule(core.device, triangle_vertex_shader, nullptr);

    global_deletion_stack.push_function([this]() {
        vkDestroyPipelineLayout(core.device, triangle_pipeline.layout, nullptr);
        vkDestroyPipeline(core.device, triangle_pipeline.pipeline, nullptr);
        });
}
void gf::VkManager::init_mesh_pipeline() {
    VkShaderModule triangle_frag_shader;
    if (!vk_pipe::load_shader_module("../../shaders/colored_tri.frag.spv", core.device, &triangle_frag_shader))
        std::cout << "Error when building the triangle fragment shader module\n";

    VkShaderModule triangle_vertex_shader;
    if (!vk_pipe::load_shader_module("../../shaders/colored_tri_mesh.vert.spv", core.device, &triangle_vertex_shader))
        std::cout << "Error when building the triangle vertex shader module\n";

    VkPushConstantRange buffer_range{};
    buffer_range.offset = 0;
    buffer_range.size = sizeof(GPUDrawPushConstants);
    buffer_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo pipeline_layout_info = vk_init::pipeline_layout_info();
    pipeline_layout_info.pPushConstantRanges = &buffer_range;
    pipeline_layout_info.pushConstantRangeCount = 1;

    vkCreatePipelineLayout(core.device, &pipeline_layout_info, nullptr, &mesh_pipeline.layout);

    vk_pipe::PipelineBuilder pipe_builder;

    pipe_builder.set_shaders(triangle_vertex_shader, triangle_frag_shader)
        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .set_polygon_mode(VK_POLYGON_MODE_FILL)
        .set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
        .set_multisampling_none()
        .enable_blending_alphablend()
        .enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL)
        .set_color_attachment_format(drawn_image.image_format)
        .set_depth_format(depth_image.image_format);
    pipe_builder.pipeline_layout = mesh_pipeline.layout;

    mesh_pipeline.pipeline = pipe_builder.build_pipeline(core.device);

    vkDestroyShaderModule(core.device, triangle_frag_shader, nullptr);
    vkDestroyShaderModule(core.device, triangle_vertex_shader, nullptr);

    global_deletion_stack.push_function([this]() {
        vkDestroyPipelineLayout(core.device, mesh_pipeline.layout, nullptr);
        vkDestroyPipeline(core.device, mesh_pipeline.pipeline, nullptr);
        });
}

// Direct copy and paste, review later
void gf::VkManager::init_imgui(GLFWwindow* window) {

    VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imguiPool;
    vkCreateDescriptorPool(core.device, &pool_info, nullptr, &imguiPool);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = core.instance;
    init_info.PhysicalDevice = core.gpu;
    init_info.Device = core.device;
    init_info.Queue = core.graphics_queue;
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.UseDynamicRendering = true;
    init_info.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
    init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchain.swapchain_format;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);
    ImGui_ImplVulkan_CreateFontsTexture();

    global_deletion_stack.push_function([=]() {
        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(core.device, imguiPool, nullptr);
        });
}

void gf::VkManager::init_default_data() {

    uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
    white_image = create_image((void*)&white, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
    uint32_t gray = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
    gray_image = create_image((void*)&gray, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
    uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
    black_image = create_image((void*)&black, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
    uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
    std::array<uint32_t, 16 * 16> pixels;
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
        }
    }
    error_checkerboard_image = create_image(pixels.data(), VkExtent3D{16, 16, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

    VkSamplerCreateInfo sampl{ .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
    sampl.magFilter = VK_FILTER_NEAREST;
    sampl.minFilter = VK_FILTER_NEAREST;
    vkCreateSampler(core.device, &sampl, nullptr, &default_sampler_nearest);
    sampl.magFilter = VK_FILTER_LINEAR;
    sampl.minFilter = VK_FILTER_LINEAR;
    vkCreateSampler(core.device, &sampl, nullptr, &default_sampler_linear);

    vk_mat::GLTFMetallic_Roughness::MaterialResources material_resources;
    material_resources.color_image = white_image;
    material_resources.color_sampler = default_sampler_linear;
    material_resources.metal_rough_image = white_image;
    material_resources.metal_rough_sampler = default_sampler_linear;
    AllocatedBuffer material_constants = create_buffer(sizeof(vk_mat::GLTFMetallic_Roughness::MaterialConstants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    vk_mat::GLTFMetallic_Roughness::MaterialConstants* scene_uniform_data = (vk_mat::GLTFMetallic_Roughness::MaterialConstants*)material_constants.allocation->GetMappedData();
    scene_uniform_data->color_factors = glm::vec4{ 1,1,1,1 };
    scene_uniform_data->metal_rough_factors = glm::vec4{ 1,0.5,0,0 };
    material_resources.data_buffer = material_constants.buffer;
    material_resources.data_buffer_offset = 0;

    default_data = metal_rough_material.write_material(core.device, MaterialPass::MainColor, material_resources, global_descriptor_allocator);

    test_meshes = vk_loader::load_gltf_meshes(this, "..\\..\\assets\\basicmesh.glb").value();



    for (auto& m : test_meshes) {
        std::shared_ptr<vk_render::MeshNode> new_node = std::make_shared<vk_render::MeshNode>();
        new_node->mesh = m;
    
        new_node->local_transform = glm::mat4{ 1.f };
        new_node->world_transform = glm::mat4{ 1.f };
    
        for (auto& s : new_node->mesh->surfaces) {
            s.material = std::make_shared<vk_loader::GLTFMaterial>(default_data);
        }
    
        loaded_nodes[m->name] = std::move(new_node);
    }

    global_deletion_stack.push_function([material_constants, this]() {
        for (auto it = test_meshes.begin(); it != test_meshes.end(); it++) {
            destroy_buffer(it->get()->mesh_buffers.index_buffer);
            destroy_buffer(it->get()->mesh_buffers.vertex_buffer);
        }
        destroy_buffer(material_constants);

        vkDestroySampler(core.device, default_sampler_nearest, nullptr);
        vkDestroySampler(core.device, default_sampler_linear, nullptr);

        destroy_image(white_image);
        destroy_image(gray_image);
        destroy_image(black_image);
        destroy_image(error_checkerboard_image);
        
        });
}

gf::AllocatedBuffer gf::VkManager::create_buffer(size_t allocation_size, VkBufferUsageFlags flags, VmaMemoryUsage memory_usage) {
    VkBufferCreateInfo buffer_info = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    buffer_info.pNext = nullptr;
    buffer_info.size = allocation_size;
    buffer_info.usage = flags;

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = memory_usage;
    vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    AllocatedBuffer new_buffer;
    vmaCreateBuffer(alloc.allocator, &buffer_info, &vmaallocInfo, &new_buffer.buffer, &new_buffer.allocation, &new_buffer.info);

    return new_buffer;
}
void gf::VkManager::destroy_buffer(const AllocatedBuffer& buffer) {
    vmaDestroyBuffer(alloc.allocator, buffer.buffer, buffer.allocation);
}
gf::GPUMeshBuffers gf::VkManager::upload_mesh(std::span<uint32_t> indices, std::span<Vertex> vertices) {
    const size_t index_buffer_size = indices.size() * sizeof(uint32_t);
    const size_t vertex_buffer_size = vertices.size() * sizeof(Vertex);
    GPUMeshBuffers new_mesh;
    new_mesh.vertex_buffer = create_buffer(vertex_buffer_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
        | VK_BUFFER_USAGE_TRANSFER_DST_BIT
        | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY);

    new_mesh.index_buffer = create_buffer(index_buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY);

    VkBufferDeviceAddressInfo device_address_info{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = new_mesh.vertex_buffer.buffer };
    new_mesh.vertex_buffer_address = vkGetBufferDeviceAddress(core.device, &device_address_info);

    AllocatedBuffer staging_buffer = create_buffer(vertex_buffer_size + index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
    void* data = staging_buffer.allocation->GetMappedData();
    memcpy(data, vertices.data(), vertex_buffer_size);
    memcpy((char*)data + vertex_buffer_size, indices.data(), index_buffer_size);

    immediate_submit([&](VkCommandBuffer cmd) {
        VkBufferCopy vertex_copy{ 0 };
        vertex_copy.dstOffset = 0;
        vertex_copy.srcOffset = 0;
        vertex_copy.size = vertex_buffer_size;
        vkCmdCopyBuffer(cmd, staging_buffer.buffer, new_mesh.vertex_buffer.buffer, 1, &vertex_copy);

        VkBufferCopy index_copy{ 0 };
        index_copy.dstOffset = 0;
        index_copy.srcOffset = vertex_buffer_size;
        index_copy.size = index_buffer_size;
        vkCmdCopyBuffer(cmd, staging_buffer.buffer, new_mesh.index_buffer.buffer, 1, &index_copy);

        });

    destroy_buffer(staging_buffer);

    return new_mesh;
}

void gf::VkManager::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function) {
    imm_frame.immediate_submit(std::move(function));
}

void gf::VkManager::resize_swapchain(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(core.device);
    
    swapchain.destroy_swapchain();

    swapchain.remake_swapchain(width, height);
    
    resize_requested = false;
}

gf::AllocatedImage gf::VkManager::create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) {
    AllocatedImage new_image;
    new_image.image_format = format;
    new_image.image_size = size;
    VkImageCreateInfo img_info = vk_init::image_info(format, size, usage);
    if (mipmapped)
        img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vmaCreateImage(alloc.allocator, &img_info, &alloc_info, &new_image.image, &new_image.allocation, nullptr);
    
    VkImageAspectFlags aspect_flag = VK_IMAGE_ASPECT_COLOR_BIT;
    if (format == VK_FORMAT_D32_SFLOAT)
        aspect_flag = VK_IMAGE_ASPECT_DEPTH_BIT;

    VkImageViewCreateInfo view_info = vk_init::image_view_info(format, new_image.image, aspect_flag);
    view_info.subresourceRange.levelCount = img_info.mipLevels;
    vkCreateImageView(core.device, &view_info, nullptr, &new_image.image_view);

    return new_image;
}
gf::AllocatedImage gf::VkManager::create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) {
    size_t data_size = size.depth * size.width * size.height * 4;
    AllocatedBuffer upload_buffer = create_buffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    memcpy(upload_buffer.info.pMappedData, data, data_size);

    AllocatedImage new_image = create_image(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);
    
    immediate_submit([new_image, upload_buffer, &size](VkCommandBuffer cmd) {
        transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkBufferImageCopy copy_region = {};
        copy_region.bufferOffset = 0;
        copy_region.bufferRowLength = 0;
        copy_region.bufferImageHeight = 0;
        copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_region.imageSubresource.mipLevel = 0;
        copy_region.imageSubresource.baseArrayLayer = 0;
        copy_region.imageSubresource.layerCount = 1;
        copy_region.imageExtent = size;

        vkCmdCopyBufferToImage(cmd, upload_buffer.buffer, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

        transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        });

    destroy_buffer(upload_buffer);

    return new_image;

}
void gf::VkManager::destroy_image(const AllocatedImage& img) {
    vkDestroyImageView(core.device, img.image_view, nullptr);
    vmaDestroyImage(alloc.allocator, img.image, img.allocation);
}

float gf::Camera::pitch = 0.f;
float gf::Camera::yaw = 0.f;
glm::vec3 gf::Camera::position{};
glm::vec3 gf::Camera::velocity{};

glm::mat4 gf::Camera::get_view_matrix() {
    glm::mat4 camera_translation = glm::translate(glm::mat4(1.f), position);
    glm::mat4 camera_rotation = get_rotation_matrix();
    return glm::inverse(camera_translation * camera_rotation);
}
glm::mat4 gf::Camera::get_rotation_matrix() {
    glm::quat pitch_rotation = glm::angleAxis(pitch, glm::vec3{ 1.f, 0.f, 0.f });
    glm::quat yaw_rotation = glm::angleAxis(yaw, glm::vec3{ 0.f, -1.f, 0.f });
    return glm::toMat4(yaw_rotation) * glm::toMat4(pitch_rotation);
}

void gf::Camera::glfw_camera_callback(gl::Key* key) {
    
    if (key->action == GLFW_PRESS) {
        switch (key->key) {
        case GLFW_KEY_W:
            Camera::velocity.z = -1;
            break;
        case GLFW_KEY_S:
            Camera::velocity.z = 1;
            break;
        case GLFW_KEY_A:
            Camera::velocity.x = -1;
            break;
        case GLFW_KEY_D:
            Camera::velocity.x = 1;
            break;
        }
    }
    if (key->action == GLFW_RELEASE) {
        switch (key->key) {
        case GLFW_KEY_W:
            Camera::velocity.z = 0;
            break;
        case GLFW_KEY_S:
            Camera::velocity.z = 0;
            break;
        case GLFW_KEY_A:
            Camera::velocity.x = 0;
            break;
        case GLFW_KEY_D:
            Camera::velocity.x = 0;
            break;
        }
    }
}

void gf::Camera::glfw_camera_mouse(gl::WInputContext* context) {
    if (context->mouse.first_mouse) {
        gl::Double2D new_pos = context->mouse.get_mouse_coords();
        yaw = static_cast<float>(new_pos.x);
        pitch = static_cast<float>(new_pos.y);
        context->mouse.first_mouse = false;
    }
    gl::Double2D offsets = context->mouse.get_mouse_offset();

    yaw += static_cast<float>(offsets.x) * 0.00075;
    pitch -= static_cast<float>(offsets.y) * 0.00075;
}

void gf::Camera::update() {
    glm::mat4 camera_rotation = get_rotation_matrix();
    position += glm::vec3(camera_rotation * glm::vec4(velocity * 0.25f, 0.f));
}