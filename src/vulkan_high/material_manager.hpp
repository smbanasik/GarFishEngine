/**
* @file
* @brief Material manager to simplify material creation.
* @author Spencer Banasik
*/
#ifndef GF_MAT_MANAGER_HPP
#define GF_MAT_MANAGER_HPP

#include <unordered_map>
#include <memory>

#include <vulkan_high/imaterial.hpp>

namespace gf {
class VkManager;
}

namespace vkh_mat {

typedef vkh_mat::IBaseMaterial Material;

/**
* @class MaterialManager
* @brief Used to automatically initialize and build materials.
* @author Spencer Banasik
* @details Used to automatically initialize and build materials.
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
    gf::VkManager* engine;
    std::unordered_map<std::string, std::unique_ptr<Material>> materials;
};
}
#endif