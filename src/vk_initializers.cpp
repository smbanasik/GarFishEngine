// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 12/17/2024
#include <vk_initializers.hpp>

#include<vulkan/vulkan.h>


VkCommandPoolCreateInfo gf::vk_init::command_pool_info(uint32_t queue_family, VkCommandPoolCreateFlags flags) {
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.pNext = nullptr;
    pool_info.flags = flags;
    pool_info.queueFamilyIndex = queue_family;
    return pool_info;
}
VkCommandBufferAllocateInfo gf::vk_init::command_allocate_info(VkCommandPool pool) {
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.pNext = nullptr;
    alloc_info.commandPool = pool;
    alloc_info.commandBufferCount = 1;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    return alloc_info;
}
VkFenceCreateInfo gf::vk_init::fence_info(VkFenceCreateFlags flags) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}
VkSemaphoreCreateInfo gf::vk_init::semaphore_info(VkSemaphoreCreateFlags flags) {
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    return info;
}
VkCommandBufferBeginInfo gf::vk_init::begin_command(VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    info.pInheritanceInfo = nullptr;
    info.flags = flags;
    return info;
}
VkImageSubresourceRange gf::vk_init::subresource_range(VkImageAspectFlags flags) {
    VkImageSubresourceRange range = {};
    range.aspectMask = flags;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;
    return range;
}
VkSemaphoreSubmitInfo gf::vk_init::submit_semaphore(VkSemaphore semaphore, VkPipelineStageFlags2 flags) {
    VkSemaphoreSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    info.pNext = nullptr;
    info.semaphore = semaphore;
    info.stageMask = flags;
    info.deviceIndex = 0;
    info.value = 1;
    return info;
}
VkCommandBufferSubmitInfo gf::vk_init::submit_command(VkCommandBuffer cmd) {
    VkCommandBufferSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    info.pNext = nullptr;
    info.commandBuffer = cmd;
    info.deviceMask = 0;
    return info;
}
VkSubmitInfo2 gf::vk_init::submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signal_semaphore, VkSemaphoreSubmitInfo* wait_semaphore) {
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
VkPresentInfoKHR gf::vk_init::present_info(VkSwapchainKHR* swapchain, VkSemaphore* wait_semaphore, uint32_t* image_idx) {
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.pSwapchains = swapchain;
    present_info.swapchainCount = 1;
    present_info.pWaitSemaphores = wait_semaphore;
    present_info.waitSemaphoreCount = 1;
    present_info.pImageIndices = image_idx;
    return present_info;
}
VkImageCreateInfo gf::vk_init::image_info(VkFormat format, VkExtent3D extent, VkImageUsageFlags flags) {
    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = format;
    info.extent = extent;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT; // MSAA
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = flags;
    return info;
}
VkImageViewCreateInfo gf::vk_init::image_view_info(VkFormat format, VkImage image, VkImageAspectFlags flags) {
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.image = image;
    info.format = format;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.subresourceRange.aspectMask = flags;
    return info;
}
VkRenderingAttachmentInfo gf::vk_init::attachment_info(VkImageView view, VkClearValue* clear, VkImageLayout layout) {
    VkRenderingAttachmentInfo color_attachment{};
    color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.pNext = nullptr;
    color_attachment.imageView = view;
    color_attachment.imageLayout = layout;
    color_attachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    if (clear) {
        color_attachment.clearValue = *clear;
    }
    return color_attachment;
}
VkRenderingInfo gf::vk_init::rendering_info(VkExtent2D render_extent, VkRenderingAttachmentInfo* color_attachment, VkRenderingAttachmentInfo* depth_attachment) {
    VkRenderingInfo render_info{};
    render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    render_info.pNext = nullptr;
    render_info.renderArea = VkRect2D{ VkOffset2D { 0, 0 }, render_extent };
    render_info.layerCount = 1;
    render_info.colorAttachmentCount = 1;
    render_info.pColorAttachments = color_attachment;
    render_info.pDepthAttachment = depth_attachment;
    render_info.pStencilAttachment = nullptr;
    return render_info;
}