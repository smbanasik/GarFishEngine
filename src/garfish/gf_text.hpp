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

#include <vkl_images.hpp>
#include <gfu_types.hpp>
#include <gf_resource_manager.hpp>
#include <vkh_renderable.hpp>
#include <vkh_materials.hpp>

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
    glm::vec2 texture_position;
    glm::ivec2 size;
    glm::ivec3 padding;
};
struct Font {
    Texture font_image;
    std::array<Character, 128> char_set;
};
class TextBox {
    friend class TextManager;
public:

    // TODO: better options for modification. Appending or recording what's changed
    // could save work
    void set_text_buffer(const std::string& text_buffer) {
        this->text_buffer = text_buffer;
        text_buffer_changed = true;
    }
    const std::string* get_text_buffer() const { return &text_buffer; };

    // TODO: text assemble options go here
    vk_render::MeshAsset* assemble_text_data();

private:
    std::string text_buffer;
    vk_render::MeshAsset text_data;
    Font* font;
    bool text_buffer_changed;
};
class TextManager {
public:

    TextManager(gf::VkManager* engine, vk_img::ImageBufferAllocator* allocator);
    ~TextManager();

    void add_font_from_file(const std::string& font_name, const std::string& font_path, unsigned int pix_size = 48); // TODO: turn this into point instead of pixel
    const Font* get_font(const std::string& font_name) { return &fonts.at(font_name); };

    TextBox* create_textbox(const std::string& text_box_name, const std::string& text_buffer = "");
    TextBox* get_textbox(const std::string& text_box_name) { return &text_boxes.at(text_box_name); };

private:

    static TextManager* singleton;
    TextManager(const TextManager& other) = delete;
    TextManager& operator=(const TextManager& other) = delete;

    vk_img::ImageBufferAllocator* font_allocator;
    vk_desc::DescriptorAllocatorGrowable* text_desc_allocator;
    vk_mat::MaterialImage* text_material;
    VkManager* creator;

    TextBox initialize_textbox(const std::string& text_box_name, const std::string& text_buffer);

    std::unordered_map<std::string, Font> fonts;
    std::unordered_map<std::string, TextBox> text_boxes;
};
}
}
#endif
