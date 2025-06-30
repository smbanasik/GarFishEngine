// Spencer Banasik
// Created: 1/16/2025
// Last Modified: 1/16/2025
#include <gf_text.hpp>

#include <cassert>
#include <vector>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H 

#include <gf_vulkan.hpp>
#include <com_grid.hpp>

static FT_Library ft_lib;
gf::text::TextManager* gf::text::TextManager::singleton = nullptr;

bool helper_init_face(FT_Face& font, const std::string& font_path, unsigned int pix_size) {
    if (FT_New_Face(ft_lib, font_path.c_str(), 0, &font))
        return false;
    FT_Set_Pixel_Sizes(font, 0, pix_size);
    return true;
}

gf::text::Font helper_convert_face_to_texture(FT_Face face, vk_img::ImageBufferAllocator* allocator) {
    const int CHAR_AMOUNT = 128;
    gf::text::Font new_font;

    int total_width = 0;
    int max_height = 0;

    for (unsigned char c = 33; c < CHAR_AMOUNT; c++) {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        total_width += face->glyph->bitmap.width;
        if (max_height < face->glyph->bitmap.rows)
            max_height = face->glyph->bitmap.rows;
    }

    new_font.font_image.image_size.width = total_width;
    new_font.font_image.image_size.height = max_height;
    new_font.font_image.image_size.depth = 1;
               
    com::Grid<unsigned char> data_buffer(total_width);
    data_buffer.reserve_rows(max_height);
    for (int i = 0; i < max_height; i++)
        data_buffer.add_empty_row();

    int current_width = 0;
    for (unsigned char c = 33; c < CHAR_AMOUNT; c++) {
        FT_Load_Char(face, c, FT_LOAD_RENDER);

        new_font.char_set[c].texture_position = { (1.0 * current_width) / total_width, 0.0f };
        new_font.char_set[c].size = { face->glyph->bitmap.width, face->glyph->bitmap.rows };
        new_font.char_set[c].padding = { face->glyph->bitmap_left, face->glyph->bitmap_top, face->glyph->advance.x };

        for (int i = 0, x = current_width, y = 0; i < face->glyph->bitmap.width * face->glyph->bitmap.rows; i++, x++) {
            data_buffer.get_elem(glm::ivec2(x, y)) = *(face->glyph->bitmap.buffer + i);
            if (x >= current_width + face->glyph->bitmap.width) {
                x = current_width;
                y++;
            }
        }
        current_width += face->glyph->bitmap.width;
    }

    new_font.font_image = allocator->create_image((void*)data_buffer.data(), new_font.font_image.image_size, VK_FORMAT_R8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, false);

    return new_font;
}


gf::text::TextManager::TextManager(gf::VkManager* engine, vk_img::ImageBufferAllocator* allocator)
    : font_allocator(allocator) {
    assert(singleton == nullptr);
    singleton = this;
    
    text_material = nullptr;
    text_desc_allocator = nullptr;
    creator = nullptr;

    FT_Init_FreeType(&ft_lib);
}
gf::text::TextManager::~TextManager() {

}
void gf::text::TextManager::add_font_from_file(const std::string& font_name, const std::string& font_path, unsigned int pix_size) {
    FT_Face face;
    if(!helper_init_face(face, font_path, pix_size))
        throw;

    Font new_font = helper_convert_face_to_texture(face, font_allocator);
    fonts[font_name] = new_font;

    FT_Done_Face(face);
}

gf::text::TextBox* gf::text::TextManager::create_textbox(const std::string& name, const std::string& text_buffer) {
    TextBox text = initialize_textbox(name, text_buffer);
    text_boxes[name] = text;
    return &text_boxes.at(name);
}

gf::text::TextBox gf::text::TextManager::initialize_textbox(const std::string& text_box_name, const std::string& text_buffer) {
    TextBox text_box;

    text_box.text_buffer_changed = false;
    text_box.text_data.name = text_box_name;

    vk_render::GeoSurface text_surface;
    text_surface.start_idx = 0;

    MaterialPass pass = MaterialPass::MainColor;

    vk_mat::MaterialImage::MaterialResources font_resources(*font_allocator);
    font_resources.color_sampler = creator->default_sampler_linear;
    font_resources.color_image = this->fonts.at("arial").font_image;
    text_surface.material->data = text_material->write_material(creator->core.device, pass, font_resources, *text_desc_allocator);

    if (text_buffer != "") {
        text_box.assemble_text_data();
    }

    text_box.text_data.surfaces.push_back(text_surface);

    return text_box;
}

// TODO: need to handle \n chars
gf::vk_render::MeshAsset* gf::text::TextBox::assemble_text_data() {
    // TODO: need to do indices, surface bounds, surface count, and verts here

    float x = 0, y = 0;
    int idx = 0;
    for (auto letter : text_buffer) {
        
        Character character_info = font->char_set[letter];

        float width = character_info.size.x;
        float height = character_info.size.y;

        float xpos = x + character_info.padding.x;
        float ypos = y - (height - character_info.padding.y);

        float atlas_width = width / font->font_image.image_size.width;
        float atlas_height = height / font->font_image.image_size.height;

        vk_render::Quad char_quad = vk_render::Quad::generate_textured_quad(idx, {xpos, ypos, width, height}, { character_info.texture_position.x, character_info.texture_position.y, atlas_width, atlas_height });
        // TODO: convert quad into transitional info

        x += character_info.padding.z;
        idx++;
    }

    // TODO: setup buffers here

    
    return &this->text_data;
}