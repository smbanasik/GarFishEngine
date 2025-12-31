/**
 * @file
 * @brief Manager for Fonts
 * @author Spencer Banasik
 */
#ifndef VKH_TEXTHIGH_HPP
#define VKH_TEXTHIGH_HPP

#include <array>
#include <string>
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <t_alloc_img.hpp>
#include <t_desc_allocgrow.hpp>
#include <vkl_imgbuf_alloc.hpp>
#include <vkl_mat_types.hpp>

namespace vkh_mat {
class MaterialImage;
}

namespace vkh_textlow {

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

/**
 * @struct Font
 * @brief Holds the character data of a Font
 * instance
 */
struct FontData {

    /**
     * @brief Placeholder material for text rendering
     */
    typedef vkh_mat::MaterialImage FontMaterial;

    std::array<Character, 128> chars;

    static void create_font_texture(FontData& font,
                                    vkl_res::ImageBufferAllocator& allocator);
    static void generate_font_spacings(FontData& font);
    static void generate_material_instance(FontData& font, FontMaterial& material);
};

/**
 * @class FontManager
 * @brief Responsible for creating and holding fonts.
 * @author Spencer Banasik
 */
class FontManager {
  public:
    // TODO: rule of 3/5/0

    FontData* load_font(std::string& name, std::string& path);
    void unload_font(std::string& name);

    FontData* get_font(std::string& name);

  private:
    std::unordered_map<std::string, vkl_res::AllocatedImage> images;
    std::unordered_map<std::string, Font> fonts;
    vkl_res::ImageBufferAllocator image_allocator;
    vkl_desc::DescriptorAllocatorGrowable descriptor_allocator;
    Font::FontMaterial* font_material;
};

} // namespace vkh_textlow
#endif
