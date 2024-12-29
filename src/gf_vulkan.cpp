// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 12/27/2024
#include <gf_vulkan.hpp>

#include <vector>
#include <iostream>
#include <cmath>

#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <vk_initializers.hpp>
#include <engine_types.hpp>
#include <vk_types.hpp>

gf::VkManager* gf::VkManager::loaded_vk = nullptr;

gf::VkManager::VkManager(GLFWwindow* window, uint32_t width, uint32_t height) {
    assert(loaded_vk == nullptr);
    loaded_vk = this;

    init_vulkan(window);
    create_allocator();
    create_swapchain(width, height);
    create_framedata();
    init_descriptors();
    init_pipelines();
    init_imgui(window);

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

    VkExtent3D image_size = { width, height, 1 };
    drawn_image.image_format = VK_FORMAT_R16G16B16A16_SFLOAT;
    drawn_image.image_size = image_size;
    VkImageUsageFlags drawn_image_usage{};
    drawn_image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawn_image_usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawn_image_usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawn_image_usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo image_info = vk_init::image_info(drawn_image.image_format, image_size, drawn_image_usage);
    VmaAllocationCreateInfo image_alloc_info = {};
    image_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    image_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vmaCreateImage(allocator, &image_info, &image_alloc_info, &drawn_image.image, &drawn_image.allocation, nullptr);
    VkImageViewCreateInfo view_info = vk_init::image_view_info(drawn_image.image_format, drawn_image.image, VK_IMAGE_ASPECT_COLOR_BIT);
    vkCreateImageView(device, &view_info, nullptr, &drawn_image.image_view);


    global_deletion_stack.push_function([this]() {
        vkDestroySwapchainKHR(this->device, this->swapchain.swapchain, nullptr);

        for (auto it = this->swapchain.swapchain_image_views.begin(); it != this->swapchain.swapchain_image_views.end(); it++) {
            vkDestroyImageView(this->device, *it, nullptr);
        }
        vkDestroyImageView(this->device, this->drawn_image.image_view, nullptr);
        vmaDestroyImage(this->allocator, this->drawn_image.image, this->drawn_image.allocation);
        });

}

void gf::VkManager::create_framedata() {

    VkCommandPoolCreateInfo pool_info = vk_init::command_pool_info(graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkFenceCreateInfo fence_info = vk_init::fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphore_info = vk_init::semaphore_info();

    for (auto it = active_frames.begin(); it != active_frames.end(); it++) {

        vkCreateCommandPool(device, &pool_info, nullptr, &(*it).command_pool);
        VkCommandBufferAllocateInfo alloc_info = vk_init::command_allocate_info((*it).command_pool);
        vkAllocateCommandBuffers(device, &alloc_info, &(*it).command_buffer);

        vkCreateFence(device, &fence_info, nullptr, &(*it).render_fence);
        vkCreateSemaphore(device, &semaphore_info, nullptr, &(*it).swapchain_semaphore);
        vkCreateSemaphore(device, &semaphore_info, nullptr, &(*it).render_semaphore);
    }

    global_deletion_stack.push_function([this]() {
        for (auto it = this->active_frames.begin(); it != this->active_frames.end(); it++) {
            vkDestroyCommandPool(device, (*it).command_pool, nullptr);
            vkDestroyFence(device, (*it).render_fence, nullptr);
            vkDestroySemaphore(device, (*it).render_semaphore, nullptr);
            vkDestroySemaphore(device, (*it).swapchain_semaphore, nullptr);
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

    VkDescriptorImageInfo img_info{};
    img_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    img_info.imageView = drawn_image.image_view;

    VkWriteDescriptorSet drawn_image_write = {};
    drawn_image_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    drawn_image_write.pNext = nullptr;

    drawn_image_write.dstBinding = 0;
    drawn_image_write.dstSet = drawn_image_descriptors;
    drawn_image_write.descriptorCount = 1;
    drawn_image_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    drawn_image_write.pImageInfo = &img_info;

    vkUpdateDescriptorSets(device, 1, &drawn_image_write, 0, nullptr);

    global_deletion_stack.push_function([this]() {
        global_descriptor_allocator.destroy_pool(device);
        vkDestroyDescriptorSetLayout(device, drawn_image_descriptor_layout, nullptr);
        });
}

void gf::VkManager::init_pipelines() {
    init_background_pipelines();
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