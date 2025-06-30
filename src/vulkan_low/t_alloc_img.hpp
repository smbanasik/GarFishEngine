/**
 * @file
 * @author Spencer Banasik
 */
#ifndef T_ALLOC_IMG_HPP
#define T_ALLOC_IMG_HPP

#include <utility>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <t_refcount.hpp>

namespace vkl_res {

class ImageBufferAllocator;

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
    ~AllocatedImage();
};
}
#endif
