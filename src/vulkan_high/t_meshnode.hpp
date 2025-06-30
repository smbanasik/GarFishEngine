/**
* @file
* @author Spencer Banasik
* @relates IRenderable
*/
#ifndef T_MESHNODE_HPP
#define T_MESHNODE_HPP

#include <t_node.hpp>

#include <memory>

#include <glm/mat4x4.hpp>

#include <vkl_types.hpp>
#include <vkh_render_types.hpp>

namespace vkh_render {

/**
 * @struct MeshNode
 * @brief A specialized node which has a MeshAsset.
 */
struct MeshNode : public Node {
    std::shared_ptr<vkh_render::MeshAsset> mesh;
    virtual void draw(const glm::mat4& top_matrix, gf::DrawContext& ctx);
};
}
#endif
