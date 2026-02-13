/**
 * @file
 * @brief Manager for Fonts
 * @author Spencer Banasik 
 */
#ifndef VKH_TEXTHIGH_HPP
#define VKH_TEXTHIGH_HPP

#include <string>
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vulkan_low/alloc_img.hpp>
#include <vulkan_low/desc_allocgrow.hpp>
#include <vulkan_low/imgbuf_alloc.hpp>
#include <vulkan_low/mat_types.hpp>

#include <vulkan_text/font_data.hpp>
#include <vulkan_text/font_image.hpp>

struct FT_LibraryRec_;
typedef struct FT_LibraryRec_* FT_Library;

namespace vkh_font_manager {

struct Font {
    vkh_font_data::FontData* font_data = nullptr;
    vkh_font_image::FontImage* font_image = nullptr;
};

/**
 * @class FontManager
 * @brief Responsible for creating and holding fonts.
 * @author Spencer Banasik
 * @pre The FreeType library must be initialized before usage.
 * @pre The ft_lib variable must be initialized to point to the ft_library before usage.
 */
class FontManager {
  public:
    // TODO: rule of 3/5/0
    // TODO: allow for font sizes

    static FT_Library* ft_lib;

    Font load_font(const std::string& name, const std::string& path);
    void unload_font(const std::string& name);

    Font get_font(const std::string& name);

  private:
    std::unordered_map<std::string, vkh_font_image::FontImage> font_textures;
    std::unordered_map<std::string, vkh_font_data::FontData> font_spacings;
    vkl_res::ImageBufferAllocator image_allocator;
};

} // namespace vkh_font_manager
#endif
