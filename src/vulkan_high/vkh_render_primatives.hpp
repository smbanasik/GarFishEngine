/**
* @file
* @author Spencer Banasik
* @relates Quad
* @todo Adjust file name the next time we use this
*/
#ifndef VKH_RENDER_PRIMATIVE_HPP
#define VKH_RENDER_PRIMATIVE_HPP

#include <array>
#include<stdint.h>

#include <glm/vec4.hpp>

#include<vkl_types.hpp>

namespace vkh_render {

/**
 * @struct Quad
 * @brief A rectangle of vertices with their respective indexes.
 * @author Spencer Banasik
 */
struct Quad {
public:
    std::array<gf::Vertex, 4> verts;
    std::array<uint32_t, 6> indices = { 0, 1, 2, 2, 3, 0 };

    /**
     * @brief Creates a textured rectangle
     * @param idx_offset [in] Offset for indexes
     * @param vert_rect  [in] Position of rectangle
     * @param texture_rect [in] Texture coordinates
     * @return A Quad fitted with the parameters
     * @todo details
     */
    static Quad generate_textured_quad(uint32_t idx_offset, glm::vec4 vert_rect, glm::vec4 texture_rect);
};

}
#endif
