/**
 * @file
 * @brief File containing font data interface
 * @author Spencer Banasik
 */
#ifndef VKH_FONT_DATA_HPP
#define VKH_FONT_DATA_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

typedef struct FT_FaceRec_ *FT_Face;

namespace vkh_font_data {
/**
 * @struct Character
 * @brief Contains the texture position and spacing data for a character
 */
struct Character {
  /**
   * @brief Position of character in atlas texture.
   * @details XY integer coordinate of where the character's texture is in a
   * font atlas texture.
   */
  glm::vec2 texture_position;

  /**
   * @brief Size of the character in pixels.
   * @details XY integer size of the character in pixels. Data sourced from the
   * Freetype library, where X is the number of columns (width) and Y is the
   * number of rows (height).
   */
  glm::ivec2 size;

  /**
   * @brief Character spacings in pixels.
   * @details XYZ integers of the character's spacings in pixels. Data sourced
   * from the Freetype library, where X is the padding left (space from origin
   * to character), Y is the height of the character from the origin (not
   * absolute height) and Z is the spacing from one origin to the next.
   */
  glm::ivec3 padding;
};

struct FontData {
  std::array<Character, 128> chars;
};

FontData create_font_spacings(FT_Face &face);

} // namespace vkh_font_data
#endif
