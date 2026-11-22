#include <vulkan_high/renderer.hpp>

#include <iostream>

#include <tracy/Tracy.hpp>

#include <vulkan_high/debug_ui.hpp>
#include <vulkan_low/initializers.hpp>
#include <window_input/wi_manager.hpp>

#include <vulkan/vk_enum_string_helper.h>
#define VK_CHECK(x)                                                                                                    \
    do {                                                                                                               \
        VkResult err = x;                                                                                              \
        if (err) {                                                                                                     \
            std::cout << "Detected Vulkan error: " << string_VkResult(err) << "\n";                                    \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

gs::Renderer::Renderer(gs::GraphicsContext& graphics_context, gf::wi::Extent2D dims)
    : swapchain(&graphics_context.core, dims.width, dims.height), frame_data(&graphics_context.core),
      frame_renderer(graphics_context, dims), frame_number(0) {}

void gs::Renderer::resize_swapchain(VkDevice device, gf::wi::Extent2D dims) {

    if (dims.width == 0 || dims.height == 0)
        return;

    vkDeviceWaitIdle(device);

    swapchain.destroy_swapchain();
    swapchain.remake_swapchain(dims.width, dims.height);

    window_resize_requested = false;
}

void gs::Renderer::render_frame(gs::GraphicsContext& graphics_context, gf::wi::Extent2D dims) {

    ZoneScopedN("Renderer::render_frame");

    frame_renderer.update_scene(dims);

    // Synchronization - Wait until frame is ready
    VK_CHECK(vkWaitForFences(graphics_context.core.device, 1, &get_current_frame().render_fence, true, 1000000000));
    VK_CHECK(vkResetFences(graphics_context.core.device, 1, &get_current_frame().render_fence));

    get_current_frame().deletion_stack.flush();
    get_current_frame().frame_descriptors.clear_pools(graphics_context.core.device);

    // Acquire Swapchain - get the swapchain image
    uint32_t swapchain_image_idx;
    VkResult err = vkAcquireNextImageKHR(graphics_context.core.device, swapchain.swapchain, 1000000000,
                                         get_current_frame().swapchain_semaphore, nullptr, &swapchain_image_idx);
    if (err == VK_ERROR_OUT_OF_DATE_KHR) {
        window_resize_requested = true;
        return;
    }

    // Command Buffer - Reset command buffer and get ready for submission
    VkCommandBuffer cmd = get_current_frame().command_buffer;
    VK_CHECK(vkResetCommandBuffer(cmd, 0));
    VkCommandBufferBeginInfo cmd_begin_info = gf::vk_init::begin_command(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

    // Image - Set up draw image
    draw_size.width
        = std::min(swapchain.swapchain_extent.width, frame_renderer.draw_image.image_size.width) * render_scale;
    draw_size.height
        = std::min(swapchain.swapchain_extent.height, frame_renderer.draw_image.image_size.height) * render_scale;
    vkl_img::transition_image(cmd, frame_renderer.draw_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    // Action - perform actual commands

    frame_renderer.draw_background(graphics_context, cmd);

    vkl_img::transition_image(cmd, frame_renderer.draw_image.image, VK_IMAGE_LAYOUT_GENERAL,
                              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    vkl_img::transition_image(cmd, frame_renderer.depth_image.image, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    frame_renderer.draw_geometry(graphics_context, cmd, &get_current_frame());

    // Transition and Copy - transition both images to optimal layout and copy image
    vkl_img::transition_image(cmd, frame_renderer.draw_image.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkl_img::transition_image(cmd, swapchain.swapchain_images[swapchain_image_idx], VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    vkl_img::copy_image_to_image(cmd, frame_renderer.draw_image.image, swapchain.swapchain_images[swapchain_image_idx],
                                 draw_size, swapchain.swapchain_extent);

    vkl_img::transition_image(cmd, swapchain.swapchain_images[swapchain_image_idx],
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    // Imgui - Draw imgui
    // debug_ui::draw_imgui(vk_context, cmd, vk_context.swapchain.swapchain_image_views[swapchain_image_idx]);

    // Final Transition - transition transition swapchain image to presentation
    vkl_img::transition_image(cmd, swapchain.swapchain_images[swapchain_image_idx],
                              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // Submission and presentation - submit buffer, semaphores, and queue, then present
    VK_CHECK(vkEndCommandBuffer(cmd));
    VkCommandBufferSubmitInfo cmd_info = gf::vk_init::submit_command(cmd);
    VkSemaphoreSubmitInfo wait_info = gf::vk_init::submit_semaphore(
        get_current_frame().swapchain_semaphore, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);
    VkSemaphoreSubmitInfo signal_info
        = gf::vk_init::submit_semaphore(get_current_frame().render_semaphore, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);
    VkSubmitInfo2 submit = gf::vk_init::submit_info(&cmd_info, &signal_info, &wait_info);
    VK_CHECK(vkQueueSubmit2(graphics_context.core.graphics_queue, 1, &submit, get_current_frame().render_fence));
    VkPresentInfoKHR present_info
        = gf::vk_init::present_info(&swapchain.swapchain, &get_current_frame().render_semaphore, &swapchain_image_idx);
    VkResult present_res = vkQueuePresentKHR(graphics_context.core.graphics_queue, &present_info);
    if (present_res == VK_ERROR_OUT_OF_DATE_KHR)
        window_resize_requested = true;

    frame_number++;
    FrameMark;
}

void gs::Renderer::temp_init_debug_ui(gs::GraphicsContext& graphics_context, gf::wi::WIManager& wi_manager,
                                      gf::wi::WInputContext& wi_context) {

    // gf::debug_ui::init_imgui(*this, wi_manager.get_window(&wi_context));
}