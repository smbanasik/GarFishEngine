/**
* @file
* @author Spencer Banasik
* @relates IRenderable
*/
#ifndef VK_IRENDERABLE_HPP
#define VK_IRENDERABLE_HPP

#include <vulkan_low/types.hpp>

namespace vkh_render {
/**
 * @struct IRenderable
 * @brief Interface for any renderable objects.
 */
struct IRenderable {
    virtual void draw(const glm::mat4& top_matrix, vkl::DrawContext& ctx) = 0;
};

}
#endif
