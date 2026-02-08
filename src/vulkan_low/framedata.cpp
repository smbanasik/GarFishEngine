#include <vulkan_low/framedata.hpp>

#include <vulkan_low/core.hpp>
#include <vulkan_low/initializers.hpp>

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

        std::vector<vkl_desc::DescriptorAllocatorGrowable::PoolSizeRatio> frame_pool_sizes = {
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 }
        };

        it->frame_descriptors = vkl_desc::DescriptorAllocatorGrowable{};
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