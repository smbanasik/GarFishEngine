// Spencer Banasik
// Created: 1/14/2025
// Last Modified: 1/14/2025
// Description:
// Contains types for an engine that an end user may use
#ifndef GF_TYPES_HPP
#define GF_TYPES_HPP
#include <array>

#include <glm/vec2.hpp>

#include <vk_types.hpp>
namespace gf {
struct TextureAtlas {
    AllocatedImage texture;
    uint32_t subdivisions_x;
    uint32_t subdivisions_y;

    std::array<glm::vec2, 4> get_texture_square(glm::ivec2 subdivision_coords);
};

}
#endif