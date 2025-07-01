/**
 * @file
 * @author Spencer Banasik
 */
#ifndef VKL_IMGBUF_ALLOC_HPP
#define VKL_IMGBUF_ALLOC_HPP

#include <vulkan/vulkan.h>
#include <t_alloc_img.hpp>
#include <t_alloc_buf.hpp>

namespace vkl_core {
class VKCore;
class VMAAllocWrapper;
}
namespace vkl_frames {
class ImmediateFrame;
}

namespace vkl_res {
/**
* @class ImageBufferAllocator
* @brief An allocator which creates AllocatedImage and AllocatedBuffer objects.
* @author Spencer Banasik
* @todo Implement details
* @related AllocatedImage
* @related AllocatedBuffer
*/
class ImageBufferAllocator {
    friend struct AllocatedImage;
    friend struct AllocatedBuffer;
public:
    ImageBufferAllocator(vkl_core::VKCore* core_handle,
        vkl_core::VMAAllocWrapper* alloc_handle, vkl_frames::ImmediateFrame* imm_handle)
        : core_handle(core_handle), alloc_handle(alloc_handle), imm_handle(imm_handle) {};

    AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false) const;
    AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false) const;
    AllocatedBuffer create_buffer(size_t allocation_size, VkBufferUsageFlags flags, VmaMemoryUsage memory_usage) const;

    void destroy_image(const AllocatedImage& image) const;
    void destroy_buffer(const AllocatedBuffer& buffer) const;

private:
    vkl_core::VKCore* core_handle;
    vkl_core::VMAAllocWrapper* alloc_handle;
    vkl_frames::ImmediateFrame* imm_handle;
};
}
#endif
