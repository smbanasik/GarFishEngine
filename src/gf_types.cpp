// Spencer Banasik
// Created: 1/14/2025
// Last Modified: 1/14/2025
#include <gf_types.hpp>

std::array<glm::vec2, 4> gf::TextureAtlas::get_texture_square(glm::ivec2 subdivision_coords) {
    if (subdivision_coords.x >= subdivisions_x || subdivision_coords.y >= subdivisions_y)
        throw;

    // Given coordinates, use the image size and subdivisions to capture a box of a texture atlast, and return the 4 corners of that box.
    std::array<glm::vec2, 4> corners;
    corners[0].x = subdivision_coords.x       * (static_cast<float>(texture->image_size.width) / static_cast<float>(subdivisions_x)) / static_cast<float>(texture->image_size.width);
    corners[0].y = (subdivision_coords.y + 1) * (static_cast<float>(texture->image_size.height) / static_cast<float>(subdivisions_y)) / static_cast<float>(texture->image_size.height);

    corners[1].x = (subdivision_coords.x + 1) * (static_cast<float>(texture->image_size.width) / static_cast<float>(subdivisions_x)) / static_cast<float>(texture->image_size.width);
    corners[1].y = (subdivision_coords.y + 1) * (static_cast<float>(texture->image_size.height) / static_cast<float>(subdivisions_y)) / static_cast<float>(texture->image_size.height);

    corners[2].x = (subdivision_coords.x + 1) * (static_cast<float>(texture->image_size.width) / static_cast<float>(subdivisions_x)) / static_cast<float>(texture->image_size.width);
    corners[2].y = subdivision_coords.y       * (static_cast<float>(texture->image_size.height) / static_cast<float>(subdivisions_y)) / static_cast<float>(texture->image_size.height);

    corners[3].x = subdivision_coords.x       * (static_cast<float>(texture->image_size.width) / static_cast<float>(subdivisions_x)) / static_cast<float>(texture->image_size.width);
    corners[3].y = subdivision_coords.y       * (static_cast<float>(texture->image_size.height) / static_cast<float>(subdivisions_y)) / static_cast<float>(texture->image_size.height);

    return corners;
}