#ifndef GS_FRAME_RENDERER_HPP
#define GS_FRAME_RENDERER_HPP

#include <vulkan/vulkan.h>

#include <garfish/camera.hpp>
#include <vulkan_high/graphics_context.hpp>
#include <vulkan_low/graphics_context/imgbuf_alloc.hpp>
#include <vulkan_low/images.hpp>
#include <vulkan_low/types.hpp>
#include <window_input/wi_types.hpp>

namespace gs {
class FrameRenderer {
  public:
    FrameRenderer(GraphicsContext& graphics_context, gf::wi::Extent2D dims);
    ~FrameRenderer();

    FrameRenderer(const FrameRenderer& other) = delete;
    FrameRenderer operator=(const FrameRenderer& other) = delete;

    void draw() {};

    void update_scene() {};

    vkl_res::AllocatedImage draw_image;
    vkl_res::AllocatedImage depth_image;
    VkDescriptorSet draw_image_descriptors;
    VkDescriptorSetLayout draw_image_descriptor_layout;

    vkl::DrawContext main_draw_context;

    // TODO: figure out where this goes.
    gf::Camera camera;

  private:
    VkDevice device_handle;
};

} // namespace gs
#endif