#include <vulkan_high/node.hpp>

void vkh_render::Node::refresh_transform(const glm::mat4& parent_matrix) {
    world_transform = parent_matrix * local_transform;
    for (auto child : children)
        child->refresh_transform(world_transform);
}
void vkh_render::Node::draw(const glm::mat4& top_matrix, vkl::DrawContext& ctx) {
    for (auto child : children)
        child->draw(top_matrix, ctx);
}