#include <vulkan_high/font_manager.hpp>

#include <iostream>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vulkan_high/font_data.hpp>
#include <vulkan_high/font_image.hpp>

FT_Library* vkh_font_manager::FontManager::ft_lib = nullptr;

bool init_face(FT_Library ft_lib, FT_Face& font, const std::string& font_path,
               unsigned int pix_size = 48) {
    FT_Error err = FT_New_Face(ft_lib, font_path.c_str(), 0, &font);
    if (err) {
        std::cerr << "FT_New_Face failed with error " << err
                  << " for file: " << font_path.c_str() << "\n";
        return false;
    }
    FT_Set_Pixel_Sizes(font, 0, pix_size);
    return true;
};

vkh_font_manager::Font vkh_font_manager::FontManager::get_font(const std::string& name) {
    Font new_font;
    new_font.font_data = &font_spacings.at(name);
    new_font.font_image = &font_textures.at(name);
    return new_font;
}

vkh_font_manager::Font vkh_font_manager::FontManager::load_font(const std::string& name,
                                                                const std::string& path) {

    FT_Face face;
    bool success = init_face(*FontManager::ft_lib, face, path);

    // TODO: If !success

    font_spacings.try_emplace(name, vkh_font_data::create_font_spacings(face));
    font_textures.try_emplace(name,
                              vkh_font_image::create_font_image(face, &image_allocator));

    return get_font(name);
}

void vkh_font_manager::FontManager::unload_font(const std::string& name) {
    font_textures.erase(name);
    font_spacings.erase(name);
}
