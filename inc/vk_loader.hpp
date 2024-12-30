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

#include <vk_types.hpp>

namespace gf {

class VkManager;
namespace vk_loader {
// Used for submeshes within a gltf.
struct GeoSurface {
    uint32_t start_idx;
    uint32_t count;
};
struct MeshAsset {
    std::string name;
    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers mesh_buffers;
};

std::optional<std::vector<std::shared_ptr<MeshAsset>>> load_gltf_meshes(VkManager* vk_context, std::filesystem::path filepath);

}
}
#endif