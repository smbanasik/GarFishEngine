#ifndef VK_RENDERER_HPP
#define VK_RENDERER_HPP

#include <stdint.h>

#include <vulkan/vulkan.h>

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
    void resize_swapchain(VkDevice device, gf::wi::Extent2D dims);

    void render_frame(gs::GraphicsContext& graphics_context, gf::wi::Extent2D dims);

    bool is_window_resize_requested() { return window_resize_requested; }
    uint64_t get_frame_number() { return frame_number; }
    vkl_frames::Frame& get_current_frame() {
        return frame_data.active_frames[frame_number % vkl_frames::FRAME_OVERLAP];
    }

    vkl_frames::SwapChain swapchain;
    vkl_frames::FrameData frame_data;
    gs::FrameRenderer frame_renderer;
    uint64_t frame_number;
    VkExtent2D draw_size{};
    float render_scale = 1.f;

    // TODO: need to figure out best way to integrate this
    // TODO: need to update debug_ui to not use the vk_context
    void temp_init_debug_ui(gs::GraphicsContext& graphics_context, gf::wi::WIManager& wi_manager,
                            gf::wi::WInputContext& wi_context);
    void temp_setup_debug_ui_frame(gs::GraphicsContext& graphics_context) {};
    void temp_run_debug_ui_frame() {};

  private:
    bool window_resize_requested = false;
};

} // namespace gs
#endif