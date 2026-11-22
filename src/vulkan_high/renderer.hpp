#ifndef VK_RENDERER_HPP
#define VK_RENDERER_HPP

#include <vulkan_high/frame_renderer.hpp>
#include <vulkan_high/graphics_context.hpp>
#include <vulkan_low/framedata.hpp>
#include <vulkan_low/graphics_context/core.hpp>
#include <vulkan_low/graphics_context/imgbuf_alloc.hpp>
#include <vulkan_low/imm_frame.hpp>
#include <vulkan_low/swapchain.hpp>
#include <window_input/wi_types.hpp>

namespace gs {

class Renderer {
  public:
    Renderer(gs::GraphicsContext& graphics_context, gf::wi::Extent2D dims);

    /**
     * @brief Update the swapchain.
     * @param [in] width New width.
     * @param [in] height New height.
     * @returns Whether the resize occured or not.
     */
    bool resize_swapchain(VkDevice device, gf::wi::Extent2D dims);

    void render_frame() {};

    vkl_frames::SwapChain swapchain;
    vkl_frames::FrameData frame_data;
    gs::FrameRenderer frame_renderer;

    bool enable_debug_ui = true;
};

} // namespace gs
#endif