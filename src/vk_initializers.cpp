// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 12/17/2024
#include <vk_initializers.hpp>

#include<vulkan/vulkan.h>

VkCommandPoolCreateInfo GF::init_vk_command_pool_info(uint32_t queue_family, VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.pNext = nullptr;
    pool_info.flags = flags;
    pool_info.queueFamilyIndex = queue_family;
    return pool_info;
}
VkCommandBufferAllocateInfo GF::init_vk_command_allocate_info(VkCommandPool pool) {
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.commandPool = pool;
    alloc_info.commandBufferCount = 1;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    return alloc_info;
}
VkFenceCreateInfo GF::init_vk_fence_info(VkFenceCreateFlags flags) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}
VkSemaphoreCreateInfo GF::init_vk_semaphore_info(VkSemaphoreCreateFlags flags) {
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}
VkCommandBufferBeginInfo GF::init_vk_begin_command(VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}