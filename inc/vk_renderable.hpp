// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
// Description:
// Higher level abstractions for rendered objects
#ifndef VK_RENDERABLE_HPP
#define VK_RENDERABLE_HPP

#include <memory>
#include <vector>
#include <string>

#include <glm/mat4x4.hpp>

#include <vk_types.hpp>

namespace gf {
namespace vk_render {
struct GLTFMaterial {
    MaterialInstance data;
};
struct GeoSurface {
    uint32_t start_idx;
    uint32_t count;
    Bounds bounds; // TODO: need to create a version of surfaces without this
    std::shared_ptr<GLTFMaterial> material;
};
struct MeshAsset {
    std::string name;
    std::vector<GeoSurface> surfaces;
    GPUMeshBuffers mesh_buffers;
};
struct DrawContext {
    std::vector<RenderObject> opaque_surfaces;
    std::vector<RenderObject> transparent_surfaces;
    std::vector<RenderObject> static_surfaces; // Meant for UI elements, ideally not affected by render proj matrix. No culling.
};

struct IRenderable {
    virtual void draw(const glm::mat4& top_matrix, DrawContext& ctx) = 0;
};

struct Node : public IRenderable {
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;

    glm::mat4 local_transform;
    glm::mat4 world_transform;

    void refresh_transform(const glm::mat4& parent_matrix);
    virtual void draw(const glm::mat4& top_matrix, DrawContext& ctx);
};
struct MeshNode : public Node {
    std::shared_ptr<MeshAsset> mesh;
    virtual void draw(const glm::mat4& top_matrix, DrawContext& ctx);
};

}
}
#endif
