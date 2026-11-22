/**
 * @file
 * @brief Higher level resource managers.
 * @author Spencer Banasik
 */
#ifndef GF_RESOURCE_MANAGER_HPP
#define GF_RESOURCE_MANAGER_HPP
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <vulkan_low/images.hpp>

namespace vkh {
class VkManager;

class ImageManager {
  public:
    ImageManager(vkl_res::ImageBufferAllocator& allocator) : textures(), allocator(&allocator) {}
    ImageManager(const ImageManager& other) = delete;
    ImageManager& operator=(const ImageManager& other) = delete;
    ~ImageManager() {};

    vkl_res::AllocatedImage* add_texture(const std::string& texture_name, const vkl_res::AllocatedImage& texture);
    vkl_res::AllocatedImage* add_texture_from_data(const std::string& texture_name, void* data, VkExtent3D size);
    vkl_res::AllocatedImage* add_texture_from_file(const std::string& texture_name, const std::string& file_path);
    vkl_res::AllocatedImage* get_texture(const std::string& texture_name);
    // TODO: delete texture

  private:
    vkl_res::ImageBufferAllocator* allocator;
    std::unordered_map<std::string, vkl_res::AllocatedImage> textures;
};
} // namespace vkh
#endif