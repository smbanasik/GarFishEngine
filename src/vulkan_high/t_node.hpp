/**
* @file
* @author Spencer Banasik
* @relates Node
*/
#ifndef T_NODE_HPP
#define T_NODE_HPP

#include <vkh_irenderable.hpp>

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>

#include <vkl_types.hpp>

namespace vkh_render {

/**
 * @struct Node
 * @brief A node in the scene tree
 */
struct Node : public IRenderable {
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;

    glm::mat4 local_transform;
    glm::mat4 world_transform;

    void refresh_transform(const glm::mat4& parent_matrix);
    virtual void draw(const glm::mat4& top_matrix, vkl::DrawContext& ctx);
};

}
#endif
