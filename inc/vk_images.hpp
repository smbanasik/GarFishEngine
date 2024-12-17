// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 12/17/2024
// Description:
// Vulkan 1.3 introduces _dynamic rendering_, which allows us to avoid using render passes.
// However, the swapchain image needs to be transitioned to the right layout for drawing
// and it needs to be transistioned back once we're done.
#ifndef VK_IMAGES_HPP
#define VK_IMAGES_HPP
#include <vulkan/vulkan.h>
namespace GF {
    
void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout);
}
#endif