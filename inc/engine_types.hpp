// Spencer Banasik
// Created: 12/15/2024
// Last Modified: 12/16/2024
// Description:
// Contains various types and helper functions for the engine
#ifndef GF_ENGINE_TYPES_HPP
#define GF_ENGINE_TYPES_HPP

#include <array>

#include<vulkan/vulkan.h>

#include<engine_managers.hpp>
#include<vk_initializers.hpp>

namespace GF {
// Data for a given frame

struct FrameData {
    VkCommandPool command_pool{};
    VkCommandBuffer command_buffer{};
    VkFence render_fence{};
    VkSemaphore swapchain_semaphore{}, render_semaphore{};

    // TODO: I'd like to have a destructor handle the destruction of the resources here
    // but unfortunately, I'm not sure how to integrate that yet! It needs the device.
    // We may want to slip the frames in with the VkManager ???
    // Come back to this once we've decided on how to do graphics interface
};

template<size_t S>
inline void init_command_data(VkManager& vk_context, std::array<FrameData, S>& frames) {

    VkCommandPoolCreateInfo pool_info = init_vk_command_pool_info(vk_context.graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (auto it = frames.begin(); it != frames.end(); it++) {

        vkCreateCommandPool(vk_context.device, &pool_info, nullptr, &(*it).command_pool);

        VkCommandBufferAllocateInfo alloc_info = init_vk_command_allocate_info((*it).command_pool);

        vkAllocateCommandBuffers(vk_context.device, &alloc_info, &(*it).command_buffer);
    }
};

template<size_t S>
inline void init_synch_data(VkManager& vk_context, std::array<FrameData, S>& frames) {

    VkFenceCreateInfo fence_info = init_vk_fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphore_info = init_vk_semaphore_info();
    for (auto it = frames.begin(); it != frames.end(); it++) {
        vkCreateFence(vk_context.device, &fence_info, nullptr, &(*it).render_fence);
        vkCreateSemaphore(vk_context.device, &semaphore_info, nullptr, &(*it).swapchain_semaphore);
        vkCreateSemaphore(vk_context.device, &semaphore_info, nullptr, &(*it).render_semaphore);
    }
}
}
#endif
