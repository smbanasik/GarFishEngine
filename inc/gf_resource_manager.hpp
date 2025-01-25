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
#include <memory>

#include <gf_types.hpp>

namespace gf {
class VkManager;
namespace vk_img {
class ImageBufferAllocator;
}

class ImageManager {
public:

    ImageManager(vk_img::ImageBufferAllocator& allocator)
        : textures(),
        allocator(&allocator) {}

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
    MaterialManager(gf::VkManager* engine, VkDevice* device)
        : device(device),
        engine(engine) {};

    template<class T>
    Material* create_material(const std::string& material_name) {
        T material;
        material.set_device(device);
        material.build_pipelines(engine);
        materials[material_name] = std::make_unique<T>(std::move(material));
        return materials.at(material_name).get();
    }
    Material* add_material(const std::string& material_name, std::unique_ptr<Material> sacrafice);
    Material* get_material(const std::string& material_name);

private:
    VkDevice* device;
    VkManager* engine;
    std::unordered_map<std::string, std::unique_ptr<Material>> materials;
};

}
#endif