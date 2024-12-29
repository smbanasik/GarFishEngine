// Spencer Banasik
// Created: 12/16/2024
// Last Modified: 12/17/2024
// Description:
// Code to simplify boilerplate creation of various vulkan constructs.
// These essentially act as constructors for the types below.
#ifndef VK_INITIALIZERS_HPP
#define VK_INITIALIZERS_HPP

#include<vulkan/vulkan.h>

namespace gf {
namespace vk_init {
VkCommandPoolCreateInfo command_pool_info(uint32_t queue_family, VkCommandPoolCreateFlags flags = 0);
VkCommandBufferAllocateInfo command_allocate_info(VkCommandPool pool);
VkFenceCreateInfo fence_info(VkFenceCreateFlags flags = 0);
VkSemaphoreCreateInfo semaphore_info(VkSemaphoreCreateFlags flags = 0);
VkCommandBufferBeginInfo begin_command(VkCommandBufferUsageFlags flags = 0);
VkImageSubresourceRange subresource_range(VkImageAspectFlags flags);
VkSemaphoreSubmitInfo submit_semaphore(VkSemaphore semaphore, VkPipelineStageFlags2 flags);
VkCommandBufferSubmitInfo submit_command(VkCommandBuffer cmd);
VkSubmitInfo2 submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signal_semaphore, VkSemaphoreSubmitInfo* wait_semaphore);
VkPresentInfoKHR present_info(VkSwapchainKHR* swapchain, VkSemaphore* wait_semaphore, uint32_t* image_idx);
VkImageCreateInfo image_info(VkFormat format, VkExtent3D extent, VkImageUsageFlags flags);
VkImageViewCreateInfo image_view_info(VkFormat format, VkImage image, VkImageAspectFlags flags);
VkRenderingAttachmentInfo attachment_info(VkImageView view, VkClearValue* clear, VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
VkRenderingInfo rendering_info(VkExtent2D render_extent, VkRenderingAttachmentInfo* color_attachment, VkRenderingAttachmentInfo* depth_attachment);
VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shaderModule, const char* entry_point = "main");
VkPipelineLayoutCreateInfo pipeline_layout_info();
}
}
#endif
