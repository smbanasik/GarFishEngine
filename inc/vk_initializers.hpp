// Spencer Banasik
// Created: 12/14/2024
// Last Modified: 12/15/2024
// Description:
// Code to simplify boilerplate creation of various vulkan types
// Any sort of info creation should go here.
#ifndef VK_INITIALIZERS_HPP
#define VK_INITIALIZERS_HPP

#include<vulkan/vulkan.h>

namespace GF {

inline VkCommandPoolCreateInfo init_vk_command_pool_info(uint32_t queue_family, VkCommandPoolCreateFlags flags = 0) {
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.pNext = nullptr;
    pool_info.flags = flags;
    pool_info.queueFamilyIndex = queue_family;
    return pool_info;
}
inline VkCommandBufferAllocateInfo init_vk_command_allocate_info(VkCommandPool pool) {
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.commandPool = pool;
    alloc_info.commandBufferCount = 1;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    return alloc_info;
}
inline VkFenceCreateInfo init_vk_fence_info(VkFenceCreateFlags flags = 0) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}
inline VkSemaphoreCreateInfo init_vk_semaphore_info(VkSemaphoreCreateFlags flags = 0) {
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}
inline VkCommandBufferBeginInfo init_vk_begin_command(VkCommandBufferUsageFlags flags = 0) {
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}
    
}
#endif
