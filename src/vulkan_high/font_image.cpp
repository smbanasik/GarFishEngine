#include <vulkan_high/font_image.hpp>

#include <stdint.h>

#include <ft2build.h>
#include <vulkan/vulkan.h>
#include FT_FREETYPE_H

#include <common/grid.hpp>
#include <vulkan_low/imgbuf_alloc.hpp>

vkh_font_image::FontImage vkh_font_image::create_font_image(
    FT_Face face, vkl_res::ImageBufferAllocator* allocator) {

    const int32_t CHAR_AMOUNT = 128;

    int total_width = 0;
    int max_height = 0;

    for (unsigned char c = 33; c < CHAR_AMOUNT; c++) {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        total_width += face->glyph->bitmap.width;
        if (max_height < face->glyph->bitmap.rows)
            max_height = face->glyph->bitmap.rows;
    }

    FontImage new_font_image;

    new_font_image.image_size.width = total_width;
    new_font_image.image_size.height = max_height;
    new_font_image.image_size.depth = 1;

    com::Grid<unsigned char> data_buffer(total_width);
    data_buffer.reserve_rows(max_height);
    for (int i = 0; i < max_height; i++)
        data_buffer.add_empty_row();

    int current_width = 0;
    for (unsigned char c = 33; c < CHAR_AMOUNT; c++) {
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        for (int i = 0, x = current_width, y = 0;
             i < face->glyph->bitmap.width * face->glyph->bitmap.rows; i++, x++) {
            data_buffer.get_elem(glm::ivec2(x, y)) = *(face->glyph->bitmap.buffer + i);
            if (x >= current_width + face->glyph->bitmap.width) {
                x = current_width;
                y++;
            }
        }
        current_width += face->glyph->bitmap.width;
    }

    new_font_image =
        allocator->create_image((void*)data_buffer.data(), new_font_image.image_size,
                                VK_FORMAT_R8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, false);

    return new_font_image;
}
