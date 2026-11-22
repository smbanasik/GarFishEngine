#ifndef VK_RENDERER_HPP
#define VK_RENDERER_HPP

#include <vulkan_low/framedata.hpp>
#include <vulkan_low/imm_frame.hpp>
#include <vulkan_low/swapchain.hpp>

namespace gs {

class Renderer {
    vkl_frames::SwapChain swapchain;
    vkl_frames::FrameData frame_data;
    vkl_frames::ImmediateFrame imm_frame;
    bool resize_requested = false;

    // TODO: debug ui here

    /**
     * @brief Update the swapchain.
     * @param [in] width New width.
     * @param [in] height New height.
     */
    void resize_swapchain(uint32_t width, uint32_t height);
};

} // namespace gs
#endif