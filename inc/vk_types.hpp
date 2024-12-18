// Spencer Banasik
// Created: 12/18/2024
// Last Modified: 12/18/2024
// Description:
// Data types meant for gf_vulkan
#ifndef VK_TYPES_HPP
#define VK_TYPES_HPP

#include <vector>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace gf {
// All swapchain information bundled together
struct SwapChain {
    VkSwapchainKHR swapchain{};
    VkFormat swapchain_format{};
    VkExtent2D swapchain_extent{};
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
};
// All data for a given frame bundled together
struct FrameData {
    VkCommandPool command_pool{};
    VkCommandBuffer command_buffer{};
    VkFence render_fence{};
    VkSemaphore swapchain_semaphore{}, render_semaphore{};
};
// All data for an image bundled together
struct AllocatedImage {
    VkImage image;
    VkImageView image_view;
    VmaAllocation allocation;
    VkExtent3D image_extent;
    VkFormat image_format;
};

}
#endif
