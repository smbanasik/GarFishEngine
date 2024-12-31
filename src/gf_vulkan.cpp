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

#include <vk_initializers.hpp>
#include <engine_types.hpp>
#include <vk_types.hpp>
#include <vk_descriptors.hpp>
#include <vk_images.hpp>

gf::VkManager* gf::VkManager::loaded_vk = nullptr;

gf::VkManager::VkManager(GLFWwindow* window, uint32_t width, uint32_t height) {
    assert(loaded_vk == nullptr);
    loaded_vk = this;

    init_vulkan(window);
    create_allocator();
    init_swapchain(width, height);
    init_commands();
    init_descriptors();
    init_pipelines();
    init_imgui(window);
    init_default_data();

    is_init = true;

}
gf::VkManager::~VkManager() {

    vkDeviceWaitIdle(device);

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

void gf::VkManager::draw_geometry(VkCommandBuffer cmd, FrameData* frame) {
    AllocatedBuffer gpu_scene_data_buffer = create_buffer(sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    
    frame->deletion_stack.push_function([&gpu_scene_data_buffer, this] {
        destroy_buffer(gpu_scene_data_buffer);
        });

    GPUSceneData* scene_uniform_data = reinterpret_cast<GPUSceneData*>(gpu_scene_data_buffer.allocation->GetMappedData());
    *scene_uniform_data = scene_data;

    VkDescriptorSet global_descriptor = frame->frame_descriptors.allocate(device, gpu_scene_data_descriptor_layout);
    DescriptorWriter writer;
    writer.write_buffer(0, gpu_scene_data_buffer.buffer, sizeof(GPUSceneData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    writer.update_set(device, global_descriptor);

    glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3{ 0, 0, -5 });
    glm::mat4 projection = glm::perspective(glm::radians(70.f), static_cast<float>(drawn_size.width) / static_cast<float>(drawn_size.height), 10000.f, 0.1f);
    projection[1][1] *= -1;

    VkRenderingAttachmentInfo color_attachment = vk_init::attachment_info(drawn_image.image_view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingAttachmentInfo depth_attachment = vk_init::depth_attachment_info(depth_image.image_view, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
    VkRenderingInfo render_info = vk_init::rendering_info(drawn_size, &color_attachment, &depth_attachment);
    vkCmdBeginRendering(cmd, &render_info);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mesh_pipeline);
    
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
    
    GPUDrawPushConstants p_constants;
    p_constants.world_matrix = projection * view;
    p_constants.vertex_buffer = test_meshes[2]->mesh_buffers.vertex_buffer_address;

    vkCmdPushConstants(cmd, mesh_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &p_constants);
    vkCmdBindIndexBuffer(cmd, test_meshes[2]->mesh_buffers.index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmd, test_meshes[2]->surfaces[0].count, 1, test_meshes[2]->surfaces[0].start_idx, 0, 0);
    
    vkCmdEndRendering(cmd);
}

void gf::VkManager::init_vulkan(GLFWwindow* window) {
    vkb::InstanceBuilder builder;

    auto inst = builder.set_app_name("GarFishEngine")
        .request_validation_layers(DEBUG_USE_VALIDATION)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();

    vkb::Instance vkb_inst = inst.value();

    instance = vkb_inst.instance;
    debug_messenger = vkb_inst.debug_messenger;

    auto err = glfwCreateWindowSurface(instance, window, NULL, &surface);
    if (err) {
        std::cout << "| ERROR: surface creation failed \n";
        std::cout << err << "\n";
    }

    VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    features13.dynamicRendering = true;
    features13.synchronization2 = true;

    VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    vkb::PhysicalDevice vkb_phys_device = selector.set_minimum_version(1, 3)
        .set_required_features_13(features13)
        .set_required_features_12(features12)
        .set_surface(surface)
        .select()
        .value();

    vkb::DeviceBuilder device_builder{ vkb_phys_device };
    vkb::Device vkb_device = device_builder.build().value();

    device = vkb_device.device;
    gpu = vkb_phys_device.physical_device;

    graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

    global_deletion_stack.push_function([this]() {
        vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
        vkDestroyDevice(this->device, nullptr);
        vkb::destroy_debug_utils_messenger(this->instance, this->debug_messenger);
        vkDestroyInstance(this->instance, nullptr);
        });
}

void gf::VkManager::init_swapchain(uint32_t width, uint32_t height) {
    
    create_swapchain(width, height);

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
    vmaCreateImage(allocator, &image_info, &image_alloc_info, &drawn_image.image, &drawn_image.allocation, nullptr);
    VkImageViewCreateInfo view_info = vk_init::image_view_info(drawn_image.image_format, drawn_image.image, VK_IMAGE_ASPECT_COLOR_BIT);
    vkCreateImageView(device, &view_info, nullptr, &drawn_image.image_view);

    depth_image.image_format = VK_FORMAT_D32_SFLOAT;
    depth_image.image_size = image_size;
    VkImageUsageFlags depth_usage_flags{};
    depth_usage_flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VkImageCreateInfo depth_image_info = vk_init::image_info(depth_image.image_format, image_size, depth_usage_flags);
    vmaCreateImage(allocator, &depth_image_info, &image_alloc_info, &depth_image.image, &depth_image.allocation, nullptr);
    VkImageViewCreateInfo depth_view_info = vk_init::image_view_info(depth_image.image_format, depth_image.image, VK_IMAGE_ASPECT_DEPTH_BIT);
    vkCreateImageView(device, &depth_view_info, nullptr, &depth_image.image_view);
    global_deletion_stack.push_function([this]() {
        destroy_swapchain();

        vkDestroyImageView(this->device, this->drawn_image.image_view, nullptr);
        vmaDestroyImage(this->allocator, this->drawn_image.image, this->drawn_image.allocation);
        vkDestroyImageView(this->device, this->depth_image.image_view, nullptr);
        vmaDestroyImage(this->allocator, this->depth_image.image, this->depth_image.allocation);
        });

}

void gf::VkManager::create_swapchain(uint32_t width, uint32_t height) {
    vkb::SwapchainBuilder swapchain_builder{ gpu, device, surface };
    swapchain.swapchain_format = VK_FORMAT_B8G8R8A8_UNORM;

    vkb::Swapchain vkb_swapchain = swapchain_builder
        .set_desired_format(VkSurfaceFormatKHR{ .format = swapchain.swapchain_format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(width, height)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

    swapchain.swapchain_extent = vkb_swapchain.extent;
    swapchain.swapchain = vkb_swapchain.swapchain;
    swapchain.swapchain_images = vkb_swapchain.get_images().value();
    swapchain.swapchain_image_views = vkb_swapchain.get_image_views().value();
}

void gf::VkManager::destroy_swapchain() {
    vkDestroySwapchainKHR(this->device, this->swapchain.swapchain, nullptr);
    for (auto it = this->swapchain.swapchain_image_views.begin(); it != this->swapchain.swapchain_image_views.end(); it++) {
        vkDestroyImageView(this->device, *it, nullptr);
    }
}

void gf::VkManager::init_commands() {
    create_framedata();

    // Immediate Rendering
    VkCommandPoolCreateInfo pool_info = vk_init::command_pool_info(graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    vkCreateCommandPool(device, &pool_info, nullptr, &imm_command_pool);
    VkCommandBufferAllocateInfo cmd_alloc_info = vk_init::command_allocate_info(imm_command_pool);
    vkAllocateCommandBuffers(device, &cmd_alloc_info, &imm_command_buffer);
    VkFenceCreateInfo fence_info = vk_init::fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
    vkCreateFence(device, &fence_info, nullptr, &imm_fence);
    global_deletion_stack.push_function([this]() {
        vkDestroyCommandPool(device, imm_command_pool, nullptr);
        vkDestroyFence(device, imm_fence, nullptr);
        });
}

void gf::VkManager::create_framedata() {

    VkCommandPoolCreateInfo pool_info = vk_init::command_pool_info(graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkFenceCreateInfo fence_info = vk_init::fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphore_info = vk_init::semaphore_info();

    for (auto it = active_frames.begin(); it != active_frames.end(); it++) {

        vkCreateFence(device, &fence_info, nullptr, &(*it).render_fence);
        vkCreateSemaphore(device, &semaphore_info, nullptr, &(*it).swapchain_semaphore);
        vkCreateSemaphore(device, &semaphore_info, nullptr, &(*it).render_semaphore);

        vkCreateCommandPool(device, &pool_info, nullptr, &(*it).command_pool);
        VkCommandBufferAllocateInfo alloc_info = vk_init::command_allocate_info((*it).command_pool);
        vkAllocateCommandBuffers(device, &alloc_info, &(*it).command_buffer);

        std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frame_pool_sizes = {
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 }
        };

        it->frame_descriptors = DescriptorAllocatorGrowable{};
        it->frame_descriptors.init(device, 1000, frame_pool_sizes);
    }

    global_deletion_stack.push_function([this]() {
        for (auto it = this->active_frames.begin(); it != this->active_frames.end(); it++) {
            vkDestroyCommandPool(device, (*it).command_pool, nullptr);
            vkDestroyFence(device, (*it).render_fence, nullptr);
            vkDestroySemaphore(device, (*it).render_semaphore, nullptr);
            vkDestroySemaphore(device, (*it).swapchain_semaphore, nullptr);
            it->frame_descriptors.destroy_pools(device);
        }
        });

}

void gf::VkManager::create_allocator() {
    VmaAllocatorCreateInfo allocator_info = {};
    allocator_info.physicalDevice = gpu;
    allocator_info.device = device;
    allocator_info.instance = instance;
    allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&allocator_info, &allocator);
    global_deletion_stack.push_function([this]() {
        vmaDestroyAllocator(this->allocator);
        });
}

void gf::VkManager::init_descriptors() {

    std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}
    };

    global_descriptor_allocator.init_pool(device, 10, sizes);
    
    {
        DescriptorLayoutBuilder builder;
        builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        drawn_image_descriptor_layout = builder.build(device, VK_SHADER_STAGE_COMPUTE_BIT);
    }
    drawn_image_descriptors = global_descriptor_allocator.allocate(device, drawn_image_descriptor_layout);

    {
        DescriptorLayoutBuilder builder;
        builder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        gpu_scene_data_descriptor_layout = builder.build(device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    DescriptorWriter writer;
    writer.write_image(0, drawn_image.image_view, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    writer.update_set(device, drawn_image_descriptors);

    global_deletion_stack.push_function([this]() {
        global_descriptor_allocator.destroy_pool(device);
        vkDestroyDescriptorSetLayout(device, drawn_image_descriptor_layout, nullptr);
        vkDestroyDescriptorSetLayout(device, gpu_scene_data_descriptor_layout, nullptr);
        });
}

void gf::VkManager::init_pipelines() {
    init_background_pipelines();
    init_mesh_pipeline();
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

    vkCreatePipelineLayout(device, &compute_layout, nullptr, &gradient_pipeline_layout);

    VkShaderModule gradient_shader;
    if (!vk_pipe::load_shader_module("../../shaders/gradient_color.comp.spv", device, &gradient_shader))
        std::cout << "| ERROR: compute shader was not built.\n";
    VkShaderModule sky_shader;
    if (!vk_pipe::load_shader_module("../../shaders/sky.comp.spv", device, &sky_shader))
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

    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &compute_create_info, nullptr, &gradient.pipeline);

    compute_create_info.stage.module = sky_shader;
    ComputeEffect sky;
    sky.layout = gradient_pipeline_layout;
    sky.name = "sky";
    sky.data = {};
    sky.data.data1 = { 0.1, 0.2, 0.4, 0.97 };

    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &compute_create_info, nullptr, &sky.pipeline);

    background_effects.push_back(gradient);
    background_effects.push_back(sky);

    vkDestroyShaderModule(device, gradient_shader, nullptr);
    vkDestroyShaderModule(device, sky_shader, nullptr);

    global_deletion_stack.push_function([=]() {
        vkDestroyPipelineLayout(device, gradient_pipeline_layout, nullptr);
        vkDestroyPipeline(device, gradient.pipeline, nullptr);
        vkDestroyPipeline(device, sky.pipeline, nullptr);
        });
}

void gf::VkManager::init_mesh_pipeline() {
    VkShaderModule tri_vert_shader;
    if (!vk_pipe::load_shader_module("../../shaders/colored_tri_mesh.vert.spv", device, &tri_vert_shader))
        std::cout << "| ERROR: vertex shader was not built.\n";
    VkShaderModule tri_frag_shader;
    if (!vk_pipe::load_shader_module("../../shaders/colored_tri.frag.spv", device, &tri_frag_shader))
        std::cout << "| ERROR: fragment shader was not built.\n";

    VkPushConstantRange buffer_range{};
    buffer_range.offset = 0;
    buffer_range.size = sizeof(GPUDrawPushConstants);
    buffer_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo pipeline_layout_info = vk_init::pipeline_layout_info();
    pipeline_layout_info.pPushConstantRanges = &buffer_range;
    pipeline_layout_info.pushConstantRangeCount = 1;

    vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &mesh_pipeline_layout);

    vk_pipe::PipelineBuilder pipe_builder;
    pipe_builder.pipeline_layout = mesh_pipeline_layout;
    pipe_builder
        .set_shaders(tri_vert_shader, tri_frag_shader)
        .set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
        .set_polygon_mode(VK_POLYGON_MODE_FILL)
        .set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
        .set_multisampling_none()
        .enable_blending_additive()
        .enable_depthtest(true, VK_COMPARE_OP_GREATER_OR_EQUAL)
        .set_color_attachment_format(drawn_image.image_format)
        .set_depth_format(depth_image.image_format);
    mesh_pipeline = pipe_builder.build_pipeline(device);

    vkDestroyShaderModule(device, tri_vert_shader, nullptr);
    vkDestroyShaderModule(device, tri_frag_shader, nullptr);

    global_deletion_stack.push_function([this]() {
        vkDestroyPipelineLayout(device, mesh_pipeline_layout, nullptr);
        vkDestroyPipeline(device, mesh_pipeline, nullptr);
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
    vkCreateDescriptorPool(device, &pool_info, nullptr, &imguiPool);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance;
    init_info.PhysicalDevice = gpu;
    init_info.Device = device;
    init_info.Queue = graphics_queue;
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
        vkDestroyDescriptorPool(device, imguiPool, nullptr);
        });
}

void gf::VkManager::init_default_data() {

    uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
    white_image = create_image((void*)white, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
    uint32_t gray = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
    gray_image = create_image((void*)gray, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
    uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
    black_image = create_image((void*)black, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
    uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
    std::array<uint32_t, 16 * 16> pixels;
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
        }
    }
    error_checkerboard_image = create_image(pixels.data(), VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

    VkSamplerCreateInfo sampl{ .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
    sampl.magFilter = VK_FILTER_NEAREST;
    sampl.minFilter = VK_FILTER_NEAREST;
    vkCreateSampler(device, &sampl, nullptr, &default_sampler_nearest);
    sampl.magFilter = VK_FILTER_LINEAR;
    sampl.minFilter = VK_FILTER_LINEAR;
    vkCreateSampler(device, &sampl, nullptr, &default_sampler_linear);

    test_meshes = vk_loader::load_gltf_meshes(this, "..\\..\\assets\\basicmesh.glb").value();

    global_deletion_stack.push_function([this]() {
        for (auto it = test_meshes.begin(); it != test_meshes.end(); it++) {
            destroy_buffer(it->get()->mesh_buffers.index_buffer);
            destroy_buffer(it->get()->mesh_buffers.vertex_buffer);
        }

        vkDestroySampler(device, default_sampler_nearest, nullptr);
        vkDestroySampler(device, default_sampler_linear, nullptr);

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
    vmaCreateBuffer(allocator, &buffer_info, &vmaallocInfo, &new_buffer.buffer, &new_buffer.allocation, &new_buffer.info);

    return new_buffer;
}
void gf::VkManager::destroy_buffer(const AllocatedBuffer& buffer) {
    vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
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
    new_mesh.vertex_buffer_address = vkGetBufferDeviceAddress(device, &device_address_info);

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
    vkResetFences(device, 1, &imm_fence);
    vkResetCommandBuffer(imm_command_buffer, 0);
    VkCommandBuffer cmd = imm_command_buffer;

    VkCommandBufferBeginInfo cmd_begin_info = vk_init::begin_command(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkBeginCommandBuffer(cmd, &cmd_begin_info);
    function(cmd);
    vkEndCommandBuffer(cmd);

    VkCommandBufferSubmitInfo cmd_submit_info = vk_init::submit_command(cmd);
    VkSubmitInfo2 submit = vk_init::submit_info(&cmd_submit_info, nullptr, nullptr);
    vkQueueSubmit2(graphics_queue, 1, &submit, imm_fence);
    vkWaitForFences(device, 1, &imm_fence, true, 1000000000);
}

void gf::VkManager::resize_swapchain(uint32_t width, uint32_t height) {
    vkDeviceWaitIdle(device);
    
    destroy_swapchain();

    create_swapchain(width, height);
    
    resize_requested = false;
}

gf::AllocatedImage gf::VkManager::create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false) {
    AllocatedImage new_image;
    new_image.image_format = format;
    new_image.image_size = size;
    VkImageCreateInfo img_info = vk_init::image_info(format, size, usage);
    if (mipmapped)
        img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vmaCreateImage(allocator, &img_info, &alloc_info, &new_image.image, &new_image.allocation, nullptr);
    
    VkImageAspectFlags aspect_flag = VK_IMAGE_ASPECT_COLOR_BIT;
    if (format == VK_FORMAT_D32_SFLOAT)
        aspect_flag = VK_IMAGE_ASPECT_DEPTH_BIT;

    VkImageViewCreateInfo view_info = vk_init::image_view_info(format, new_image.image, aspect_flag);
    view_info.subresourceRange.levelCount = img_info.mipLevels;
    vkCreateImageView(device, &view_info, nullptr, &new_image.image_view);

    return new_image;
}
gf::AllocatedImage gf::VkManager::create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false) {
    size_t data_size = size.depth * size.height * size.width * 4;
    AllocatedBuffer upload_buffer = create_buffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    memcpy(upload_buffer.info.pMappedData, data, data_size);

    AllocatedImage new_image = create_image(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);
    
    immediate_submit([&new_image, &upload_buffer, &size](VkCommandBuffer cmd) {
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
    vkDestroyImageView(device, img.image_view, nullptr);
    vmaDestroyImage(allocator, img.image, img.allocation);
}