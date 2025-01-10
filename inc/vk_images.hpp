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

#include <gf_util.hpp>
namespace gf {
namespace vk_core {
class VKCore;
class Alloc;
}
namespace vk_frames {
class ImmediateFrame;
}
namespace vk_img {
class ImageBufferAllocator {
    friend class AllocatedImage;
    friend class AllocatedBuffer;
public:
    ImageBufferAllocator(vk_core::VKCore* core_handle,
        vk_core::Alloc* alloc_handle, vk_frames::ImmediateFrame* imm_handle)
        : core_handle(core_handle), alloc_handle(alloc_handle), imm_handle(imm_handle) {};

    AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
    AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
    AllocatedBuffer create_buffer(size_t allocation_size, VkBufferUsageFlags flags, VmaMemoryUsage memory_usage);
private:
    vk_core::VKCore* core_handle;
    vk_core::Alloc* alloc_handle;
    vk_frames::ImmediateFrame* imm_handle;
};

class AllocatedImage {
public:

    AllocatedImage() : image(nullptr), image_view(nullptr),
        allocation(nullptr), image_size({ 0, 0, 0 }), image_format(VK_FORMAT_UNDEFINED),
        allocator(nullptr), counter() {};

    AllocatedImage(ImageBufferAllocator* allocator) : image(nullptr), image_view(nullptr),
        allocation(nullptr), image_size({ 0, 0, 0 }), image_format(VK_FORMAT_UNDEFINED),
        allocator(allocator), counter() {};

    AllocatedImage(const AllocatedImage& other);
    AllocatedImage(AllocatedImage&& other) noexcept;
    AllocatedImage& operator=(const AllocatedImage& other) {
        if (this == &other)
            return *this;
        image = other.image;
        image_view = other.image_view;
        allocation = other.allocation;
        image_size = other.image_size;
        image_format = other.image_format;
        allocator = other.allocator;
        counter = other.counter;
        return *this;
    }
    AllocatedImage& operator=(AllocatedImage&& other) noexcept {
        if (this == &other)
            return *this;
        image = std::move(other.image);
        image_view = std::move(other.image_view);
        allocation = std::move(other.allocation);
        image_size = std::move(other.image_size);
        image_format = std::move(other.image_format);
        counter = std::move(other.counter);
        allocator = std::move(other.allocator);
        other.image = nullptr;
        other.image_view = nullptr;
        other.allocation = nullptr;
        other.allocator = nullptr;
        return *this;
    }
    ~AllocatedImage();

    VkImage image;
    VkImageView image_view;
    VmaAllocation allocation;
    VkExtent3D image_size;
    VkFormat image_format;
    util::RefCounter counter;
private:
    ImageBufferAllocator* allocator;
};

class AllocatedBuffer {
public:

    AllocatedBuffer() : buffer(nullptr), allocation(nullptr),
        info(), allocator(nullptr), counter() {};

    AllocatedBuffer(ImageBufferAllocator* allocator) : buffer(nullptr), allocation(nullptr),
        info(),
        allocator(allocator), counter() {};

    AllocatedBuffer(const AllocatedBuffer& other);
    AllocatedBuffer(AllocatedBuffer&& other) noexcept;

    AllocatedBuffer& operator=(const AllocatedBuffer& other) {
        if (this == &other)
            return *this;
        buffer = other.buffer;
        allocation = other.allocation;
        info = other.info;
        counter = other.counter;
        allocator = other.allocator;
        return *this;
    }
    AllocatedBuffer& operator=(AllocatedBuffer&& other) noexcept {
        if (this == &other)
            return *this;
        buffer = std::move(other.buffer);
        allocation = std::move(other.allocation);
        info = std::move(other.info);
        counter = std::move(other.counter);
        allocator = std::move(other.allocator);
        other.buffer = nullptr;
        other.allocation = nullptr;
        other.allocator = nullptr;
        return *this;
    }
    ~AllocatedBuffer();

    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
    util::RefCounter counter;

private:
    ImageBufferAllocator* allocator;
};

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout);
void copy_image_to_image(VkCommandBuffer cmd, VkImage src, VkImage dst, VkExtent2D src_size, VkExtent2D dst_size);
}
}
#endif
