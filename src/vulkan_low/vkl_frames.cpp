// Spencer Banasik
// Created: 1/7/2024
// Last Modified: 1/7/2025
#include <vkl_frames.hpp>

#include<vulkan/vulkan.h>
#include <VkBootstrap.h>

#include <vkl_core.hpp>
#include <vkl_initializers.hpp>
#include <vkl_types.hpp>

#include <iostream>

vkl_frames::SwapChain::SwapChain(vkl_core::VKCore* core, uint32_t width, uint32_t height)
    : core_handle(core) {
    remake_swapchain(width, height);
}
vkl_frames::SwapChain::~SwapChain() {
    destroy_swapchain();
}
vkl_frames::SwapChain::SwapChain(SwapChain&& other) noexcept
    : swapchain(std::move(other.swapchain)), swapchain_format(std::move(other.swapchain_format)),
    swapchain_images(std::move(other.swapchain_images)), swapchain_image_views(std::move(other.swapchain_image_views)),
    core_handle(std::move(other.core_handle)) {
    other.swapchain = nullptr;
    for (auto it = other.swapchain_image_views.begin(); it != swapchain_image_views.end(); it++) {
        *it = nullptr;
    }
}

void vkl_frames::SwapChain::remake_swapchain(uint32_t width, uint32_t height) {
    vkb::SwapchainBuilder swapchain_builder{ core_handle->gpu, core_handle->device, core_handle->surface };
    swapchain_format = VK_FORMAT_B8G8R8A8_UNORM;

    vkb::Swapchain vkb_swapchain = swapchain_builder
        .set_desired_format(VkSurfaceFormatKHR{ .format = swapchain_format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(width, height)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

    swapchain_extent = vkb_swapchain.extent;
    swapchain = vkb_swapchain.swapchain;
    swapchain_images = vkb_swapchain.get_images().value();
    swapchain_image_views = vkb_swapchain.get_image_views().value();
}
void vkl_frames::SwapChain::destroy_swapchain() {
    if (swapchain != nullptr)
        vkDestroySwapchainKHR(core_handle->device, swapchain, nullptr);
    for (auto it = swapchain_image_views.begin(); it != swapchain_image_views.end(); it++) {
        if (*it != nullptr)
            vkDestroyImageView(core_handle->device, *it, nullptr);
    }
}

vkl_frames::FrameData::FrameData(vkl_core::VKCore* core)
    : core_handle(core) {
    VkCommandPoolCreateInfo pool_info = gf::vk_init::command_pool_info(core_handle->graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkFenceCreateInfo fence_info = gf::vk_init::fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphore_info = gf::vk_init::semaphore_info();

    for (auto it = active_frames.begin(); it != active_frames.end(); it++) {

        vkCreateFence(core_handle->device, &fence_info, nullptr, &(*it).render_fence);
        vkCreateSemaphore(core_handle->device, &semaphore_info, nullptr, &(*it).swapchain_semaphore);
        vkCreateSemaphore(core_handle->device, &semaphore_info, nullptr, &(*it).render_semaphore);

        vkCreateCommandPool(core_handle->device, &pool_info, nullptr, &(*it).command_pool);
        VkCommandBufferAllocateInfo alloc_info = gf::vk_init::command_allocate_info((*it).command_pool);
        vkAllocateCommandBuffers(core_handle->device, &alloc_info, &(*it).command_buffer);

        std::vector<gf::vk_desc::DescriptorAllocatorGrowable::PoolSizeRatio> frame_pool_sizes = {
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 }
        };

        it->frame_descriptors = gf::vk_desc::DescriptorAllocatorGrowable{};
        it->frame_descriptors.init(core_handle->device, 1000, frame_pool_sizes);
    }
}
vkl_frames::FrameData::~FrameData() {
    for (auto it = active_frames.begin(); it != active_frames.end(); it++) {
        if ((*it).command_pool != nullptr) {
            vkDestroyCommandPool(core_handle->device, (*it).command_pool, nullptr);
            vkDestroyFence(core_handle->device, (*it).render_fence, nullptr);
            vkDestroySemaphore(core_handle->device, (*it).render_semaphore, nullptr);
            vkDestroySemaphore(core_handle->device, (*it).swapchain_semaphore, nullptr);
            it->deletion_stack.flush();
            it->frame_descriptors.destroy_pools(core_handle->device);
        }
    }
}
vkl_frames::FrameData::FrameData(FrameData&& other) noexcept
    : core_handle(std::move(other.core_handle)), active_frames(std::move(other.active_frames)) {
    for (auto it = other.active_frames.begin(); it != other.active_frames.end(); it++) {
        (*it).command_pool = nullptr;
        (*it).command_buffer = nullptr;
        (*it).render_fence = nullptr;
        (*it).render_semaphore = nullptr;
        (*it).swapchain_semaphore = nullptr;
    }

}

VkFence imm_fence;
VkCommandBuffer imm_command_buffer;
VkCommandPool imm_command_pool;

vkl_frames::ImmediateFrame::ImmediateFrame(vkl_core::VKCore* core)
: core_handle(core) {
    VkCommandPoolCreateInfo pool_info = gf::vk_init::command_pool_info(core_handle->graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    vkCreateCommandPool(core_handle->device, &pool_info, nullptr, &imm_command_pool);
    VkCommandBufferAllocateInfo cmd_alloc_info = gf::vk_init::command_allocate_info(imm_command_pool);
    vkAllocateCommandBuffers(core_handle->device, &cmd_alloc_info, &imm_command_buffer);
    VkFenceCreateInfo fence_info = gf::vk_init::fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
    vkCreateFence(core_handle->device, &fence_info, nullptr, &imm_fence);
}
vkl_frames::ImmediateFrame::~ImmediateFrame() {
    if (imm_command_pool != nullptr) {
        vkDestroyCommandPool(core_handle->device, imm_command_pool, nullptr);
        vkDestroyFence(core_handle->device, imm_fence, nullptr);
    }
}
vkl_frames::ImmediateFrame::ImmediateFrame(ImmediateFrame&& other) noexcept 
: imm_fence(std::move(other.imm_fence)), imm_command_buffer(std::move(other.imm_command_buffer)),
imm_command_pool(std::move(other.imm_command_pool)), core_handle(std::move(other.core_handle)) {
    other.imm_command_pool = nullptr;
    other.imm_fence = nullptr;
}
void vkl_frames::ImmediateFrame::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function) {
    vkResetFences(core_handle->device, 1, &imm_fence);
    vkResetCommandBuffer(imm_command_buffer, 0);
    VkCommandBuffer cmd = imm_command_buffer;

    VkCommandBufferBeginInfo cmd_begin_info = gf::vk_init::begin_command(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkBeginCommandBuffer(cmd, &cmd_begin_info);
    function(cmd);
    vkEndCommandBuffer(cmd);

    VkCommandBufferSubmitInfo cmd_submit_info = gf::vk_init::submit_command(cmd);
    VkSubmitInfo2 submit = gf::vk_init::submit_info(&cmd_submit_info, nullptr, nullptr);
    vkQueueSubmit2(core_handle->graphics_queue, 1, &submit, imm_fence);
    vkWaitForFences(core_handle->device, 1, &imm_fence, true, 1000000000);
}