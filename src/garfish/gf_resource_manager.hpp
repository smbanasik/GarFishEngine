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

/**
* @class MaterialManager
* @brief Vulkan entrypoint for engine.
* @author Spencer Banasik
* @details Used to automatically initialize3 and build materials.
* @todo Move this to a different file!
*/
class MaterialManager {
public:
    MaterialManager(gf::VkManager* engine, VkDevice* device)
        : device(device),
        engine(engine) {};

    /**
     * @brief Produces an instance of a material from the class.
     * @tparam T The material type. 
     * @param [in] material_name The name of the material.
     * @returns A pointer to the initialized material.
     */
    template<class T>
    Material* create_material(const std::string& material_name) {
        T material;
        material.set_device(device);
        material.build_pipelines(engine);
        materials[material_name] = std::make_unique<T>(std::move(material));
        return materials.at(material_name).get();
    }

    /**
     * @brief Adds a material to the manager.
     * @param [in] material_name Name of the material.
     * @param [in] sacrafice Material that is cannibalized by the manager.
     * @returns A pointer to the new material.
     */
    Material* add_material(const std::string& material_name, std::unique_ptr<Material> sacrafice);

    /**
     * @brief Gets the material by name.
     * @param [in] material_name Name of material.
     * @returns A pointer to the material.
     */
    Material* get_material(const std::string& material_name);

private:
    VkDevice* device;
    VkManager* engine;
    std::unordered_map<std::string, std::unique_ptr<Material>> materials;
};

}
#endif