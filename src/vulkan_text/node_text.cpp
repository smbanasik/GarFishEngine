#include <vulkan_high/meshnode.hpp>

void vkh_render::MeshNode::draw(const glm::mat4& top_matrix, vkl::DrawContext& ctx) {
    glm::mat4 node_matrix = top_matrix * world_transform;
    for (auto surface : mesh->surfaces) {
        vkl::RenderObject render;
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