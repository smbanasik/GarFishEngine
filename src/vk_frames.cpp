// Spencer Banasik
// Created: 1/7/2024
// Last Modified: 1/7/2025
#include <vk_frames.hpp>

#include<vulkan/vulkan.h>
#include <VkBootstrap.h>

#include <vk_core.hpp>
#include <vk_initializers.hpp>
#include <vk_types.hpp>

gf::vk_frames::SwapChain::SwapChain(vk_core::VKCore* core, uint32_t width, uint32_t height)
    : core_handle(core) {
    remake_swapchain(width, height);
}
gf::vk_frames::SwapChain::~SwapChain() {
    destroy_swapchain();
}
gf::vk_frames::SwapChain::SwapChain(SwapChain&& other) noexcept
    : swapchain(std::move(other.swapchain)), swapchain_format(std::move(other.swapchain_format)),
    swapchain_images(std::move(other.swapchain_images)), swapchain_image_views(std::move(other.swapchain_image_views)),
    core_handle(std::move(other.core_handle)) {
    other.swapchain = nullptr;
    for (auto it = other.swapchain_image_views.begin(); it != swapchain_image_views.end(); it++) {
        *it = nullptr;
    }
}

void gf::vk_frames::SwapChain::remake_swapchain(uint32_t width, uint32_t height) {
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
void gf::vk_frames::SwapChain::destroy_swapchain() {
    if (swapchain != nullptr)
        vkDestroySwapchainKHR(core_handle->device, swapchain, nullptr);
    for (auto it = swapchain_image_views.begin(); it != swapchain_image_views.end(); it++) {
        if (*it != nullptr)
            vkDestroyImageView(core_handle->device, *it, nullptr);
    }
}

gf::vk_frames::FrameData::FrameData(vk_core::VKCore* core)
    : core_handle(core) {
    VkCommandPoolCreateInfo pool_info = vk_init::command_pool_info(core_handle->graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VkFenceCreateInfo fence_info = vk_init::fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphore_info = vk_init::semaphore_info();

    for (auto it = active_frames.begin(); it != active_frames.end(); it++) {

        vkCreateFence(core_handle->device, &fence_info, nullptr, &(*it).render_fence);
        vkCreateSemaphore(core_handle->device, &semaphore_info, nullptr, &(*it).swapchain_semaphore);
        vkCreateSemaphore(core_handle->device, &semaphore_info, nullptr, &(*it).render_semaphore);

        vkCreateCommandPool(core_handle->device, &pool_info, nullptr, &(*it).command_pool);
        VkCommandBufferAllocateInfo alloc_info = vk_init::command_allocate_info((*it).command_pool);
        vkAllocateCommandBuffers(core_handle->device, &alloc_info, &(*it).command_buffer);

        std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frame_pool_sizes = {
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 }
        };

        it->frame_descriptors = DescriptorAllocatorGrowable{};
        it->frame_descriptors.init(core_handle->device, 1000, frame_pool_sizes);
    }
}
gf::vk_frames::FrameData::~FrameData() {
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
gf::vk_frames::FrameData::FrameData(FrameData&& other) noexcept
    : core_handle(std::move(other.core_handle)), active_frames(std::move(other.active_frames)) {
    for (auto it = other.active_frames.begin(); it != other.active_frames.end(); it++) {
        (*it).command_pool = nullptr;
        (*it).command_buffer = nullptr;
        (*it).render_fence = nullptr;
        (*it).render_semaphore = nullptr;
        (*it).swapchain_semaphore = nullptr;
    }

}