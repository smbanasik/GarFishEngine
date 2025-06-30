/**
* @file
* @brief Higher level types for the engine.
* @author Spencer Banasik
*/
#ifndef GF_TYPES_HPP
#define GF_TYPES_HPP
#include <array>

#include <glm/vec2.hpp>

#include <vkl_types.hpp>
#include <vkh_materials.hpp>
#include <vkh_imgatlas.hpp>
namespace gf {

typedef vk_img::AllocatedImage Texture;
typedef vk_mat::IBaseMaterial Material;

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
    const vkh::ImageAtlas* atlas;
    glm::vec2 atlas_coords;
};

}
#endif
