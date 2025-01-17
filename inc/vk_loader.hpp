// Spencer Banasik
// Created: 12/29/2024
// Last Modified: 12/29/2024
// Description:
// Handles file loading
#ifndef VK_LOADER_HPP
#define VK_LOADER_HPP

#include <filesystem>
#include <string>
#include <vector>
#include <optional>
#include <memory>

#include <vulkan/vulkan.h>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include <vk_types.hpp>
#include <vk_renderable.hpp>
#include <vk_descriptors.hpp>
#include <vk_images.hpp>

namespace gf {
class VkManager;
namespace vk_loader {

class LoadedGLTF : public vk_render::IRenderable {
public:
    std::unordered_map<std::string, std::shared_ptr<vk_render::MeshAsset>> meshes;
    std::unordered_map<std::string, std::shared_ptr<vk_render::Node>> nodes;
    std::unordered_map<std::string, vk_img::AllocatedImage> images;
    std::unordered_map<std::string, std::shared_ptr<vk_render::GLTFMaterial>> materials;

    std::vector<std::shared_ptr<vk_render::Node>> top_nodes;
    std::vector<VkSampler> samplers;

    vk_desc::DescriptorAllocatorGrowable descriptor_pool;
    vk_img::AllocatedBuffer material_data_buffer;
    VkManager* creator;
    ~LoadedGLTF() { clear_all(); };

    virtual void draw(const glm::mat4& top_matrix, vk_render::DrawContext& ctx);

private:
    void clear_all();
};

std::optional<std::shared_ptr<LoadedGLTF>> load_gltf(VkManager* engine, std::string_view file_path);
VkFilter extract_filter(fastgltf::Filter filter);
VkSamplerMipmapMode extract_mipmap_mode(fastgltf::Filter filter);
std::optional<vk_img::AllocatedImage> load_image(vk_img::ImageBufferAllocator* allocator, fastgltf::Asset& asset, fastgltf::Image& image);
std::optional<vk_img::AllocatedImage> load_image_from_path(vk_img::ImageBufferAllocator* allocator, const std::string& file_path);

}
}
#endif
