// Spencer Banasik
// Created: 1/16/2025
// Last Modified: 1/16/2025
// Description:
// Text rendering
#ifndef GF_TEXT_HPP
#define GF_TEXT_HPP

#include <unordered_map>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vk_images.hpp>
#include <gf_types.hpp>
#include <gf_resource_manager.hpp>

// TEMPORARY DOC:
// TextManager - initializes the library, stores fonts and owns text boxes(?).
// TextBox - a box of text, use as few as possible. Contains the text buffer, and any info needed to render a piece of text.
// Font - contains any information needed to create and use a font.
// Character - contains any information needed to construct a character.
// How does a user interact with this?
// The user should be able to 

namespace gf {
namespace text {
struct Character {
    glm::vec2 char_position;
    glm::ivec2 size;
    glm::ivec3 padding;
};
struct Font {
    Texture font_image;
    TextureAtlas font;
    std::array<Character, 128> char_set;
};
class TextBox {
public:
    void draw();
private:
    std::string text_buffer;
    // TODO: some render object here which stores actual render info, and font info
    bool text_buffer_changed;
private:
};
class TextManager {
public:

    TextManager(vk_img::ImageBufferAllocator* allocator);
    ~TextManager();
    // TODO: make singleton

    //void add_font(const std::string& font_name, const Font& font);
    void add_font_from_file(const std::string& font_name, const std::string& font_path, unsigned int pix_size = 48); // TODO: turn this into point instead of pixel
    const Font* get_font(const std::string& font_name) { return &fonts.at(font_name); };

    TextBox* create_textbox();
    TextBox* get_textbox(std::string name);

    void draw_text();

private:
    static TextManager* singleton;

    vk_img::ImageBufferAllocator* allocator;

    std::unordered_map<std::string, Font> fonts;
    std::unordered_map<std::string, TextBox> text_boxes;
};
}
}
#endif
