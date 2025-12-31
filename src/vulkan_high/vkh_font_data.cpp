#include <vkh_font_data.hpp>

#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H

vkh_font_data::FontData vkh_font_data::create_font_spacings(FT_Face face) {

    const int32_t CHAR_AMOUNT = 128;

    int total_width = 0;
    int max_height = 0;

    for (unsigned char c = 33; c < CHAR_AMOUNT; c++) {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        total_width += face->glyph->bitmap.width;
        if (max_height < face->glyph->bitmap.rows)
            max_height = face->glyph->bitmap.rows;
    }

    int current_width = 0;
    FontData new_font_data;

    for (unsigned char c = 33; c < CHAR_AMOUNT; c++) {
        FT_Load_Char(face, c, FT_LOAD_RENDER);

        new_font_data.chars[c].texture_position = {(1.0 * current_width) / total_width,
                                                   0.0f};
        new_font_data.chars[c].size = {face->glyph->bitmap.width,
                                       face->glyph->bitmap.rows};
        new_font_data.chars[c].padding = {
            face->glyph->bitmap_left, face->glyph->bitmap_top, face->glyph->advance.x};
        current_width += face->glyph->bitmap.width;
    }

    return new_font_data;
};
