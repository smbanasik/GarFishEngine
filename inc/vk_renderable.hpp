// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
// Description:
// Higher level abstractions for rendered objects
#ifndef VK_RENDERABLE_HPP
#define VK_RENDERABLE_HPP

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>

#include <vk_types.hpp>
#include <vk_loader.hpp>

namespace gf {
namespace vk_render {
struct DrawContext {
    std::vector<RenderObject> opaque_surfaces;
};

struct IRenderable {
    virtual void draw(const glm::mat4 top_matrix, DrawContext& ctx) = 0;
};

struct Node : public IRenderable {
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;

    glm::mat4 local_transform;
    glm::mat4 world_transform;

    void refresh_transform(const glm::mat4 parent_matrix);
    virtual void draw(const glm::mat4 top_matrix, DrawContext& ctx);
};
struct MeshNode : public Node {
    std::shared_ptr<vk_loader::MeshAsset> mesh;
    virtual void draw(const glm::mat4 top_matrix, DrawContext& ctx);
};

}
}
#endif
