// Spencer Banasik
// Created: 1/14/2025
// Last Modified: 1/14/2025
#include <gf_resource_manager.hpp>

#include <vk_images.hpp>
#include <vk_loader.hpp>

gf::ImageManager::ImageManager(ImageManager& other)
    : textures(std::move(other.textures)),
    allocator(std::move(other.allocator)) {

}
gf::ImageManager::ImageManager(ImageManager&& other) noexcept
    : textures(std::move(other.textures)),
    allocator(std::move(other.allocator)) {
}

gf::ImageManager::~ImageManager() {
    textures.clear();
}

gf::Texture* gf::ImageManager::add_texture(const std::string& texture_name, const gf::Texture& texture) {
    textures[texture_name] = texture;
    return &textures.at(texture_name);
}
gf::Texture* gf::ImageManager::add_texture_from_data(const std::string& texture_name, void* data, VkExtent3D size) {
    textures[texture_name] = allocator->create_image(data, size, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, false);
    return &textures.at(texture_name);
}
gf::Texture* gf::ImageManager::add_texture_from_file(const std::string& texture_name, const std::string& file_path) {
    textures[texture_name] = vk_loader::load_image_from_path(allocator, file_path).value();
    return &textures.at(texture_name);
}
gf::Texture* gf::ImageManager::get_texture(const std::string& texture_name) {
    return &textures.at(texture_name);
}