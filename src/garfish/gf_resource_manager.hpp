/**
* @file
* @brief Higher level resource managers.
* @author Spencer Banasik
*/
#ifndef GF_RESOURCE_MANAGER_HPP
#define GF_RESOURCE_MANAGER_HPP
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include <gfu_types.hpp>

namespace vkl_res {
class ImageBufferAllocator;
}
namespace gf {
class VkManager;

class ImageManager {
public:

    ImageManager(vkl_res::ImageBufferAllocator& allocator)
        : textures(),
        allocator(&allocator) {}

    Texture* add_texture(const std::string& texture_name, const Texture& texture);
    Texture* add_texture_from_data(const std::string& texture_name, void* data, VkExtent3D size);
    Texture* add_texture_from_file(const std::string& texture_name, const std::string& file_path);
    Texture* get_texture(const std::string& texture_name);

private:
    vkl_res::ImageBufferAllocator* allocator;
    std::unordered_map<std::string, Texture> textures;
};
}
#endif