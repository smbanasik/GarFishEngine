// Spencer Banasik
// Created: 12/16/2024
// Last Modified: 12/17/2024
// Description:
// Code to simplify boilerplate creation of various vulkan types
// Any sort of info creation should go here.
// TODO: consider splitting to .cpp and .hpp files.
#ifndef VK_INITIALIZERS_HPP
#define VK_INITIALIZERS_HPP

#include<vulkan/vulkan.h>

namespace GF {

VkCommandPoolCreateInfo init_vk_command_pool_info(uint32_t queue_family, VkCommandPoolCreateFlags flags = 0);
VkCommandBufferAllocateInfo init_vk_command_allocate_info(VkCommandPool pool);
VkFenceCreateInfo init_vk_fence_info(VkFenceCreateFlags flags = 0);
VkSemaphoreCreateInfo init_vk_semaphore_info(VkSemaphoreCreateFlags flags = 0);
VkCommandBufferBeginInfo init_vk_begin_command(VkCommandBufferUsageFlags flags = 0);
    
}
#endif
