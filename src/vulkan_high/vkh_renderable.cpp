// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
#include <vkh_renderable.hpp>

gf::vk_render::Quad gf::vk_render::Quad::generate_textured_quad(uint32_t idx_offset, glm::vec4 vert_rect, glm::vec4 texture_rect) {

    Quad quad;

    for (auto& idx : quad.indices) {
        idx += (idx_offset * 4);
    }

    for (auto& vert : quad.verts) {
        vert.normal = { 0, 0, 1 };
        vert.color = { 1, 1, 1, 1 };
    }

    quad.verts[0].position = { vert_rect.x,               vert_rect.y, 0 };
    quad.verts[1].position = { vert_rect.x + vert_rect.z, vert_rect.y, 0 };
    quad.verts[2].position = { vert_rect.x + vert_rect.z, vert_rect.y + vert_rect.w, 0 };
    quad.verts[3].position = { vert_rect.x,               vert_rect.y + vert_rect.w, 0 };

    quad.verts[0].uv_x = texture_rect.x;
    quad.verts[1].uv_x = texture_rect.x + texture_rect.z;
    quad.verts[2].uv_x = texture_rect.x + texture_rect.z;
    quad.verts[3].uv_x = texture_rect.x;

    quad.verts[0].uv_y = texture_rect.y;
    quad.verts[1].uv_y = texture_rect.y;
    quad.verts[2].uv_y = texture_rect.y + texture_rect.w;
    quad.verts[3].uv_y = texture_rect.y + texture_rect.w;

    return quad;
}

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
        render.bounds = surface.bounds;
        render.vertex_buffer_address = mesh->mesh_buffers.vertex_buffer_address;
        ctx.opaque_surfaces.push_back(render);
    }

    Node::draw(top_matrix, ctx);
}