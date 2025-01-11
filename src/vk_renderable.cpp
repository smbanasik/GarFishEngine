// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
#include <vk_renderable.hpp>

void gf::vk_render::Node::refresh_transform(const glm::mat4& parent_matrix) {
    world_transform = parent_matrix * local_transform;
    for (auto child : children)
        child->refresh_transform(world_transform);
}
void gf::vk_render::Node::draw(const glm::mat4& top_matrix, DrawContext& ctx) {
    for (auto child : children)
        child->draw(top_matrix, ctx);
}
void gf::vk_render::MeshNode::draw(const glm::mat4& top_matrix, DrawContext& ctx) {
    glm::mat4 node_matrix = top_matrix * world_transform;
    for (auto surface : mesh->surfaces) {
        RenderObject render;
        render.first_index = surface.start_idx;
        render.index_count = surface.count;
        render.index_buffer = mesh->mesh_buffers.index_buffer.buffer;
        render.material = &surface.material->data;
        render.transform = node_matrix;
        render.vertex_buffer_address = mesh->mesh_buffers.vertex_buffer_address;
        ctx.opaque_surfaces.push_back(render);
    }

    Node::draw(top_matrix, ctx);
}