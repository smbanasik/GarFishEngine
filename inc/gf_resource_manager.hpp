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

    ImageManager()
        : textures(),
        allocator() {}
    ImageManager(ImageManager&& other) noexcept;
    ImageManager& operator=(ImageManager&& other) noexcept {
        if (this == &other)
            return *this;
        textures = std::move(other.textures);
        allocator = std::move(other.allocator);
        return *this;
    }
    ~ImageManager();

    Texture* add_texture_from_file(std::string texture_name, std::string file_path);
    Texture* get_texture(std::string texture_name);

private:
    // Since we don't handle texture copying, we don't handle copying the managers
    ImageManager(ImageManager& other) = delete;
    ImageManager& operator=(ImageManager& other) = delete;

    vk_img::ImageBufferAllocator* allocator;
    std::unordered_map<std::string, Texture> textures;
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