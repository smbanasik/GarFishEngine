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
VkImageSubresourceRange GF::init_vk_subresource_range(VkImageAspectFlags flags) {
    VkImageSubresourceRange range = {};
    range.aspectMask = flags;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;
    return range;
}
VkSemaphoreSubmitInfo GF::init_vk_submit_semaphore(VkSemaphore semaphore, VkPipelineStageFlags2 flags) {
    VkSemaphoreSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    info.pNext = nullptr;
    info.semaphore = semaphore;
    info.stageMask = flags;
    info.deviceIndex = 0;
    info.value = 1;
    return info;
}
VkCommandBufferSubmitInfo GF::init_vk_submit_command(VkCommandBuffer cmd) {
    VkCommandBufferSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    info.pNext = nullptr;
    info.commandBuffer = cmd;
    info.deviceMask = 0;
    return info;
}
VkSubmitInfo2 GF::init_vk_submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signal_semaphore, VkSemaphoreSubmitInfo* wait_semaphore) {
    VkSubmitInfo2 info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    info.pNext = nullptr;
    info.waitSemaphoreInfoCount = (wait_semaphore == nullptr) ? 0 : 1;
    info.pWaitSemaphoreInfos = wait_semaphore;
    info.signalSemaphoreInfoCount = (signal_semaphore == nullptr) ? 0 : 1;
    info.pSignalSemaphoreInfos = signal_semaphore;
    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos = cmd;
    return info;
}