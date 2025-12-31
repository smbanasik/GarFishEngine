/**
 * @file
 * @brief File containing font data interface
 * @author Spencer Banasik
 */
#ifndef VKH_FONT_IMAGE_HPP
#define VKH_FONT_IMAGE_HPP

namespace vkl_res {
struct AllocatedImage;
class ImageBufferAllocator;
} // namespace vkl_res

typedef struct FT_FaceRec_* FT_Face;

namespace vkh_font_image {

// We abstract FontImage from AllocatedImage in case we want to change it later.
typedef vkl_res::AllocatedImage FontImage;

FontImage create_font_image(FT_Face& face, vkl_res::ImageBufferAllocator* allocator);

} // namespace vkh_font_image
#endif
