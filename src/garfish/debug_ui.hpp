#ifndef GF_DEBUG_UI_HPP
#define GF_DEBUG_UI_HPP

#include <vulkan/vulkan.h>

#include <garfish/vulkan.hpp>

namespace gf {
namespace debug_ui {

// TODO: redo this once our renderer is figured out.

/**
 * @brief A draw function for imgui
 * @param [in, out] cmd The command buffer to recieve draw commands.
 * @param [in] image_view
 * @todo Provide more details
 */
void draw_imgui(VkManager& vk_context, VkCommandBuffer cmd, VkImageView image_view);

void setup_imgui_frame(VkManager& vk_context);

/**
 * @brief Initialize IMGUI
 * @param [in] window Windo that IMGUI is attached to.
 */
void init_imgui(VkManager& vk_context, GLFWwindow* window);

} // namespace debug_ui
} // namespace gf

#endif