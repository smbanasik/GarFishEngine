/**
* @file
* @brief File that contains functions related to images
* @author Spencer Banasik
*/
#ifndef VKL_IMAGES_HPP
#define VKL_IMAGES_HPP

#include <string>
#include <optional>

#include <vulkan_low/alloc_img.hpp>

namespace vkl_res {
class ImageBufferAllocator;
}

namespace vkl_img {

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout);
void copy_image_to_image(VkCommandBuffer cmd, VkImage src, VkImage dst, VkExtent2D src_size, VkExtent2D dst_size);
void generate_mipmaps(VkCommandBuffer cmd, VkImage image, VkExtent2D imageSize);

std::optional<vkl_res::AllocatedImage> load_image_from_path(vkl_res::ImageBufferAllocator* allocator, const std::string& file_path);

}
#endif

