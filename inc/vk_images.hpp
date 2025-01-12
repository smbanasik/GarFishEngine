// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 1/10/2025
// Description:
// Holds information for images and buffers
#ifndef VK_IMAGES_HPP
#define VK_IMAGES_HPP
#include <utility>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <vk_types.hpp>

#include <gf_util.hpp>
namespace fastgltf {
class Asset;
struct Image;
}
namespace gf {
class VkManager;
namespace vk_core {
class VKCore;
class Alloc;
}
namespace vk_frames {
class ImmediateFrame;
}
namespace vk_img {
class ImageBufferAllocator {
public:
    ImageBufferAllocator(vk_core::VKCore* core_handle,
        vk_core::Alloc* alloc_handle, vk_frames::ImmediateFrame* imm_handle)
        : core_handle(core_handle), alloc_handle(alloc_handle), imm_handle(imm_handle) {};

    AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
    AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
    AllocatedBuffer create_buffer(size_t allocation_size, VkBufferUsageFlags flags, VmaMemoryUsage memory_usage);

    void destroy_image(const AllocatedImage& image);
    void destroy_buffer(const AllocatedBuffer& buffer);

private:
    vk_core::VKCore* core_handle;
    vk_core::Alloc* alloc_handle;
    vk_frames::ImmediateFrame* imm_handle;
};

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout);
void copy_image_to_image(VkCommandBuffer cmd, VkImage src, VkImage dst, VkExtent2D src_size, VkExtent2D dst_size);
void generate_mipmaps(VkCommandBuffer cmd, VkImage image, VkExtent2D imageSize);
}
}
#endif
