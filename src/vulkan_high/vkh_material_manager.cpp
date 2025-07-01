#include <vkh_material_manager.hpp>

vkh_mat::Material* vkh_mat::MaterialManager::add_material(const std::string& material_name, std::unique_ptr<Material> sacrafice) {
    materials[material_name] = std::move(sacrafice);
    return materials.at(material_name).get();
}
vkh_mat::Material* vkh_mat::MaterialManager::get_material(const std::string& material_name) {
    return materials.at(material_name).get();
}