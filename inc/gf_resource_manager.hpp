// Spencer Banasik
// Created: 1/14/2025
// Last Modified: 1/14/2025
// Description:
// Managers for higher level things, these express ownership over these
#ifndef GF_RESOURCE_MANAGER_HPP
#define GF_RESOURCE_MANAGER_HPP
#include <vector>
#include <unordered_map>
#include <string>

#include <gf_types.hpp>

namespace gf {
namespace vk_img {
class ImageBufferAllocator;
}

class ImageManager {
public:

    ImageManager(vk_img::ImageBufferAllocator& allocator)
        : textures(),
        allocator(&allocator) {}
    ImageManager(ImageManager& other);
    ImageManager& operator=(ImageManager& other) {
        if (this == &other)
            return *this;
        textures = other.textures;
        allocator = other.allocator;
        return *this;
    };
    ImageManager(ImageManager&& other) noexcept;
    ImageManager& operator=(ImageManager&& other) noexcept {
        if (this == &other)
            return *this;
        textures = std::move(other.textures);
        allocator = std::move(other.allocator);
        return *this;
    }
    ~ImageManager();

    Texture* add_texture(const std::string& texture_name, const Texture& texture);
    Texture* add_texture_from_data(const std::string& texture_name, void* data, VkExtent3D size);
    Texture* add_texture_from_file(const std::string& texture_name, const std::string& file_path);
    Texture* get_texture(const std::string& texture_name);

private:
    vk_img::ImageBufferAllocator* allocator;
    std::unordered_map<std::string, Texture> textures;
};
class MaterialManager {
public:
private:
};

// TODO:
// Right now there are materials and
// images hard coded into the image.
// An image manager should hold textures
// and recieve an allocator. It should manage the 
// allocation and destruction of these images

// TODO:
// A material manager does the same as the above
// but for materials

}
#endif