#include <vulkan_text/font_data.hpp>

#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H

static const unsigned char CHAR_START = 32;
static const unsigned char CHAR_AMOUNT = 128;

vkh_font_data::FontData vkh_font_data::create_font_spacings(FT_Face face) {

    int total_width = 0;
    int max_height = 0;

    for (unsigned char c = CHAR_START; c < CHAR_AMOUNT; c++) {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        total_width += face->glyph->bitmap.width;
        if (max_height < face->glyph->bitmap.rows)
            max_height = face->glyph->bitmap.rows;
    }

    int current_width = 0;
    FontData new_font_data{};

    for (unsigned char c = CHAR_START; c < CHAR_AMOUNT; c++) {
        FT_Load_Char(face, c, FT_LOAD_RENDER);

        // Grab the top left corner of our character, normalized to 1.0
        new_font_data.chars[c].texture_topleft = {static_cast<float>(current_width + 0.5) / total_width,
                                                   0.0f};
        new_font_data.chars[c].size = {face->glyph->bitmap.width,
                                       face->glyph->bitmap.rows};
        new_font_data.chars[c].padding = {
            face->glyph->bitmap_left, face->glyph->bitmap_top, face->glyph->advance.x};
        current_width += face->glyph->bitmap.width;
    }

    return new_font_data;
};
