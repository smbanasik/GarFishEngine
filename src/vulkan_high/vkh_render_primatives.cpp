#include <vkh_render_primatives.hpp>

vkh_render::Quad vkh_render::Quad::generate_textured_quad(uint32_t idx_offset, glm::vec4 vert_rect, glm::vec4 texture_rect) {

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