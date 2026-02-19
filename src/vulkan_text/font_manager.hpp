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

namespace vkl_core {
class VKCore;
class VMAAllocWrapper;
} // namespace vkl_core
namespace vkl_frames {
class ImmediateFrame;
}

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

    FontManager(vkl_core::VKCore* core_handle, vkl_core::VMAAllocWrapper* alloc_handle,
                vkl_frames::ImmediateFrame* imm_handle)
        : font_textures(), font_spacings(),
          image_allocator(core_handle, alloc_handle, imm_handle) {};
    FontManager(const FontManager& other) = delete;
    FontManager(FontManager&& other) noexcept;
    FontManager& operator=(const FontManager& other) = delete;
    FontManager& operator=(FontManager&& other) noexcept {
        if (this == &other)
            return *this;
        font_textures = std::move(other.font_textures);
        font_spacings = std::move(other.font_spacings);
        image_allocator = std::move(other.image_allocator);
        return *this;
    }
    ~FontManager() {};

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
