#ifndef GS_FRAME_RENDERER_HPP
#define GS_FRAME_RENDERER_HPP

#include <vulkan/vulkan.h>

#include <garfish/camera.hpp>
#include <vulkan_low/images.hpp>
#include <vulkan_low/types.hpp>

namespace gs {
class FrameRenderer {
  public:
    vkl_res::AllocatedImage draw_image;
    vkl_res::AllocatedImage depth_image;
    VkDescriptorSet draw_image_descriptors;
    VkDescriptorSetLayout draw_image_descriptor_layout;

    vkl::DrawContext main_draw_context;

    gf::Camera camera;

    void init_render_targets(uint32_t width, uint32_t height) {};

    void update_scene(uint32_t width, uint32_t height) {};
};

} // namespace gs
#endif