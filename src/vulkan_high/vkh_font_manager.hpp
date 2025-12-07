/**
* @file
* @brief Manager for Fonts
* @author Spencer Banasik
*/
#ifndef VKH_TEXTHIGH_HPP
#define VKH_TEXTHIGH_HPP

#include <array>
#include <unordered_map>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <t_alloc_img.hpp>
#include <vkl_mat_types.hpp>
#include <vkl_imgbuf_alloc.hpp>
#include <t_desc_allocgrow.hpp>

namespace vkh_mat {
class MaterialImage;
}

namespace vkh_textlow {

/**
 * @struct Character
 * @brief Contains the texture position and spacing data for a character
 */
struct Character {
    glm::vec2 texture_position;
    glm::ivec2 size;
    glm::ivec3 padding;
};

/**
 * @struct Font
 * @brief Holds the font's texture, character data, and the font's material instance
 */
struct Font {

    /**
    * @brief Placeholder material for text rendering
    */
    typedef vkh_mat::MaterialImage FontMaterial;

    std::string name;
    std::array<Character, 128> chars;
    vkl::MaterialInstance material;

    static void create_font_texture(Font& font, vkl_res::ImageBufferAllocator& allocator);
    static void generate_font_spacings(Font& font);
    static void generate_material_instance(Font& font, FontMaterial& material);

};

/**
 * @class FontManager
 * @brief Responsible for creating and holding fonts.
 * @author Spencer Banasik
 */
class FontManager {
public:

    // TODO: rule of 3/5/0

    Font* load_font(std::string& name, std::string& path);
    void unload_font(std::string& name);

    Font* get_font(std::string& name);

private:

    std::unordered_map<std::string, vkl_res::AllocatedImage> images;
    std::unordered_map<std::string, Font> fonts;
    vkl_res::ImageBufferAllocator image_allocator;
    vkl_desc::DescriptorAllocatorGrowable descriptor_allocator;
    Font::FontMaterial* font_material;

};

}
#endif