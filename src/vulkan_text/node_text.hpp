/**
 * @file
 * @author Spencer Banasik
 * @relates IRenderable
 */
#ifndef VULKAN_TEXT_NODE_TEXT_HPP
#define VULKAN_TEXT_NODE_TEXT_HPP

#include <vulkan_high/node.hpp>

#include <memory>

#include <glm/mat4x4.hpp>

#include <vulkan_text/types_text.hpp>
#include <vulkan_low/types.hpp>

namespace vk_text {

/**
 * @struct MeshNode
 * @brief A specialized node which has a MeshAsset.
 */
struct TextNode : public vkh_render::Node {
    std::shared_ptr<vk_text::TextAsset> mesh;
    virtual void draw(const glm::mat4& top_matrix, vkl::DrawContext& ctx);
};
} // namespace vk_text
#endif
