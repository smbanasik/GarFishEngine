/**
* @file
* @brief Higher level types for the engine.
* @author Spencer Banasik
*/
#ifndef GF_TYPES_HPP
#define GF_TYPES_HPP
#include <array>

#include <glm/vec2.hpp>

#include <vk_types.hpp>
#include <vk_materials.hpp>
namespace gf {

typedef vk_img::AllocatedImage Texture;
typedef vk_mat::IBaseMaterial Material;

/**
 * @brief Treat an image as an atlas.
 * @author Spencer Banasik
 * @todo Consider redoing this interface, right now images have to be equally spaced.
 * @details This treats a texture as an atlas of textures, like a collage of images.
 * In this case, we want to single out an image in the atlas and extract it for a specific
 * asset.
 */
struct TextureAtlas {
    const Texture* texture;
    uint32_t subdivisions_x;
    uint32_t subdivisions_y;

    std::array<glm::vec2, 4> get_texture_square(glm::ivec2 subdivision_coords) const;
};

/**
 * @brief Simple, high level interface of a sprite.
 */
struct Sprite {
    glm::vec2 sprite_position;
    glm::vec2 sprite_size;
};

/**
 * @brief A sprite with a whole texture.
 * @todo consider finding a way to merge AtlasSprite and TextureSprite.
 */
struct TextureSprite : public Sprite {
    const Texture* texture;
    glm::vec2 texture_size;
};

/**
 * @brief A sprite using a texture atlas.
 */
struct AtlasSprite : public Sprite {
    const TextureAtlas* atlas;
    glm::vec2 atlas_coords;
};

}
#endif
