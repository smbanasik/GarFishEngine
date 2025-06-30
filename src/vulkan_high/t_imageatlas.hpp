/**
* @file
* @author Spencer Banasik
* @relates ImageAtlas
*/
#ifndef VKH_IMGATLAS_HPP
#define VKH_IMGATLAS_HPP

#include <stdint.h>
#include <array>

#include <glm/vec2.hpp>

#include <vkl_images.hpp>

namespace vkh {
/**
 * @struct ImageAtlas
 * @brief Treat an image as an atlas.
 * @author Spencer Banasik
 * @todo Consider redoing this interface, right now images have to be equally spaced.
 * @details This treats an image as an atlas, like a collage of images.
 * In this case, we want to single out an image in the atlas and extract it for a specific
 * asset.
 */
struct ImageAtlas {
    const vk_img::AllocatedImage* texture;
    uint32_t subdivisions_x;
    uint32_t subdivisions_y;

    std::array<glm::vec2, 4> get_texture_square(glm::ivec2 subdivision_coords) const;
};
}
#endif
