/**
 * @file
 * @brief Related to loading gltf.
 */
#ifndef VKL_GLTF_HPP
#define VKL_GLTF_HPP

#include <filesystem>
#include <string>
#include <vector>
#include <optional>
#include <memory>

#include <vulkan/vulkan.h>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include <vkl_types.hpp>
#include <vkh_irenderable.hpp>
#include <t_node.hpp>
#include <t_meshnode.hpp>
#include <vkh_render_types.hpp>
#include <t_desc_allocgrow.hpp>
#include <vkl_images.hpp>

namespace gf {
class VkManager;
namespace vk_loader {

/**
 * @struct LoadedGLTF
 * @brief A GLTF scene.
 */
class LoadedGLTF : public vkh_render::IRenderable {
public:
    std::unordered_map<std::string, std::shared_ptr<vkh_render::MeshAsset>> meshes;
    std::unordered_map<std::string, std::shared_ptr<vkh_render::Node>> nodes;
    std::unordered_map<std::string, vkl_res::AllocatedImage> images;
    std::unordered_map<std::string, std::shared_ptr<vkh_render::GLTFMaterial>> materials;

    std::vector<std::shared_ptr<vkh_render::Node>> top_nodes;
    std::vector<VkSampler> samplers;

    vkl_desc::DescriptorAllocatorGrowable descriptor_pool;
    vkl_res::AllocatedBuffer material_data_buffer;
    VkManager* creator = nullptr;
    ~LoadedGLTF() { clear_all(); };

    virtual void draw(const glm::mat4& top_matrix, gf::DrawContext& ctx);

private:
    void clear_all();
};

std::optional<std::shared_ptr<LoadedGLTF>> load_gltf(VkManager* engine, std::string_view file_path);
VkFilter extract_filter(fastgltf::Filter filter);
VkSamplerMipmapMode extract_mipmap_mode(fastgltf::Filter filter);
std::optional<vkl_res::AllocatedImage> load_image(vkl_res::ImageBufferAllocator* allocator, fastgltf::Asset& asset, fastgltf::Image& image);

}
}
#endif
