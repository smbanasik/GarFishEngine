/**
* @file
* @author Spencer Banasik
* @relates IRenderable
*/
#ifndef T_MESHNODE_HPP
#define T_MESHNODE_HPP

#include <vulkan_high/node.hpp>

#include <memory>

#include <glm/mat4x4.hpp>

#include <vulkan_low/types.hpp>
#include <vulkan_high/render_types.hpp>

namespace vkh_render {

/**
 * @struct MeshNode
 * @brief A specialized node which has a MeshAsset.
 */
struct MeshNode : public Node {
    std::shared_ptr<vkh_render::MeshAsset> mesh;
    virtual void draw(const glm::mat4& top_matrix, vkl::DrawContext& ctx);
};
}
#endif
