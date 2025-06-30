/**
* @file
* @brief File that contains abstractions related to the Images and Buffers.
* @author Spencer Banasik
*/
#ifndef VKL_IMAGES_HPP
#define VKL_IMAGES_HPP
#include <utility>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <com_refcount.hpp>
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
    ImageBufferAllocator(vk_core::VKCore* core_handle,
        vk_core::Alloc* alloc_handle, vk_frames::ImmediateFrame* imm_handle)
        : core_handle(core_handle), alloc_handle(alloc_handle), imm_handle(imm_handle) {};

    AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false) const;
    AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false) const;
    AllocatedBuffer create_buffer(size_t allocation_size, VkBufferUsageFlags flags, VmaMemoryUsage memory_usage) const;

    void destroy_image(const AllocatedImage& image) const;
    void destroy_buffer(const AllocatedBuffer& buffer) const;

private:
    vk_core::VKCore* core_handle;
    vk_core::Alloc* alloc_handle;
    vk_frames::ImmediateFrame* imm_handle;
};

/**
 * @struct AllocatedImage
 * @brief Contains all necessary data to allocate an image.
 * @author Spencer Banasik
 * @details A RAII wrapper of an image, it's view, it's allocation,
 * and data with the image. AllocatedImages are reference counted,
 * similar to shared pointers.
 */
struct AllocatedImage {
    VkImage image;
    VkImageView image_view;
    VmaAllocation allocation;
    VkExtent3D image_size;
    VkFormat image_format;
    com::RefCounter counter;
    const ImageBufferAllocator* allocator;

    AllocatedImage()
        : image(nullptr),
        image_view(nullptr),
        allocation(nullptr),
        image_size(),
        image_format(),
        counter(),
        allocator(nullptr) {}
    AllocatedImage(const ImageBufferAllocator& allocator)
        : image(nullptr),
        image_view(nullptr),
        allocation(nullptr),
        image_size(),
        image_format(),
        counter(),
        allocator(&allocator) {}
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
        counter = other.counter;
        allocator = other.allocator;
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
        return *this;
    }
    ~AllocatedImage() {
        if (counter.can_delete_resources())
            allocator->destroy_image(*this);
    }
};

/**
 * @struct AllocatedImage
 * @brief Contains all necessary data to allocate a buffer.
 * @author Spencer Banasik
 * @details A RAII wrapper of a buffer, it's allocation,
 * and additional info. AllocatedBuffers are reference counted,
 * similar to shared pointers.
 */
struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
    com::RefCounter counter;
    const ImageBufferAllocator* allocator;

    AllocatedBuffer()
        : buffer(nullptr),
        allocation(nullptr),
        info(),
        counter(),
        allocator(nullptr) {}
    AllocatedBuffer(const ImageBufferAllocator& allocator)
        : buffer(nullptr),
        allocation(nullptr),
        info(),
        counter(),
        allocator(&allocator) {}
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
        return *this;
    }
    ~AllocatedBuffer() {
        if (counter.can_delete_resources())
            allocator->destroy_buffer(*this);
    }
};

void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout);
void copy_image_to_image(VkCommandBuffer cmd, VkImage src, VkImage dst, VkExtent2D src_size, VkExtent2D dst_size);
void generate_mipmaps(VkCommandBuffer cmd, VkImage image, VkExtent2D imageSize);
}
}
#endif

