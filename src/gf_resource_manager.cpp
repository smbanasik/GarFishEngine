// Spencer Banasik
// Created: 1/14/2025
// Last Modified: 1/14/2025
#include <gf_resource_manager.hpp>

#include <vk_images.hpp>

gf::ImageManager::ImageManager(ImageManager&& other) noexcept
    : textures(std::move(other.textures)),
    allocator(std::move(other.allocator)) {
}

gf::ImageManager::~ImageManager() {
    for (auto texture : textures) {
        allocator->destroy_image(texture.second);
    }
}