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
#include <vk_materials.hpp>
namespace gf {

typedef vk_img::AllocatedImage Texture;
typedef vk_mat::IBaseMaterial Material;

struct TextureAtlas {
    const Texture* texture;
    uint32_t subdivisions_x;
    uint32_t subdivisions_y;

    std::array<glm::vec2, 4> get_texture_square(glm::ivec2 subdivision_coords) const;
};

struct Sprite {
    glm::vec2 sprite_position;
    glm::vec2 sprite_size;
};
struct TextureSprite : public Sprite {
    const Texture* texture;
    glm::vec2 texture_size;
};
struct AtlasSprite : public Sprite {
    const TextureAtlas* atlas;
    glm::vec2 atlas_coords;
};

struct TileMap {
    // TODO: grid of sprites
};

}
#endif
