#ifndef VKH_MESH_MANAGER_HPP
#define VKH_MESH_MANAGER_HPP

#include <string>
#include <unordered_map>

#include <vulkan_high/render_types.hpp>

namespace vkh {

class MeshManager {
  public:
    MeshManager() {};
    ~MeshManager() {};
    MeshManager(const MeshManager& other) = delete;
    MeshManager& operator=(const MeshManager& other) = delete;

    void add_mesh(vkh_render::MeshAsset mesh) { meshes[mesh.name] = mesh; }
    vkh_render::MeshAsset* get_mesh(std::string name) { return &meshes[name]; }
    void delete_mesh(std::string name) { meshes.erase(name); }

  private:
    std::unordered_map<std::string, vkh_render::MeshAsset> meshes;
};

} // namespace vkh
#endif