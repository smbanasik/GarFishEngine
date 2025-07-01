// Spencer Banasik
// Created: 12/14/2024
// Last Modified: 1/6/2025
#include <gf_engine.hpp>

#include <stdint.h>
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <cmath>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <gf_vulkan.hpp>
#include <vkl_initializers.hpp>
#include <vkl_images.hpp>
// TEMPORARY
#include <vulkan/vk_enum_string_helper.h>
#define VK_CHECK(x)                                                     \
    do {                                                                \
        VkResult err = x;                                               \
        if (err) {                                                      \
            std::cout << "Detected Vulkan error: " << string_VkResult(err) << "\n"; \
            abort();                                                    \
        }                                                               \
    } while (0)

gf::Engine* gf::Engine::loaded_engine = nullptr;

gf::Engine::Engine() : wi_manager(), wi_context(wi_manager.create_window(gf::wi::WindowType::WINDOWED, {window_dims.width, window_dims.height}, title)),
vk_context(wi_manager, wi_context) {

    assert(loaded_engine == nullptr);
    loaded_engine = this;

    wi_context.window.set_callback_window_resize([this](wi::WInputContext* context) {
        this->get_vk_context()->resize_requested = true;
        wi::Extent2D dims = wi_context.window.get_window_dims();
        this->window_dims.height = dims.height;
        this->window_dims.width = dims.width;
        });

    wi_context.key.set_key_mapping(GLFW_KEY_W, Camera::glfw_camera_callback);
    wi_context.key.set_key_mapping(GLFW_KEY_S, Camera::glfw_camera_callback);
    wi_context.key.set_key_mapping(GLFW_KEY_A, Camera::glfw_camera_callback);
    wi_context.key.set_key_mapping(GLFW_KEY_D, Camera::glfw_camera_callback);
    wi_context.key.set_key_mapping(GLFW_KEY_LEFT_ALT, Camera::mouse_swap);

    wi_context.mouse.disable_cursor();
    wi_context.mouse.enable_raw_mouse();
    wi_context.mouse.set_callback_mouse_movement(Camera::glfw_camera_mouse);
}

gf::Engine::~Engine() {
    // I love RAII.
}

gf::Engine& gf::Engine::get() {
    return *loaded_engine;
}

void gf::Engine::run() {

    while (should_kill_game == false) {

        auto start = std::chrono::system_clock::now();

        // Poll events
        glfwPollEvents();


        if (should_render == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        if (vk_context.resize_requested == true)
            vk_context.resize_swapchain(window_dims.width, window_dims.height);

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();
        if (ImGui::Begin("background")) {
            ImGui::SliderFloat("Render Scale", &vk_context.render_scale, 0.3f, 1.0f);

            vkl::ComputeEffect& selected = vk_context.background_effects[vk_context.current_background_effect];
            ImGui::Text("Selected effect: ", selected.name);
            ImGui::SliderInt("Effect Index", &vk_context.current_background_effect, 0, vk_context.background_effects.size() - 1);

            ImGui::InputFloat4("data1", (float*)&selected.data.data1);
            ImGui::InputFloat4("data2", (float*)&selected.data.data2);
            ImGui::InputFloat4("data3", (float*)&selected.data.data3);
            ImGui::InputFloat4("data4", (float*)&selected.data.data4);
        }
        if (ImGui::Begin("Stats")) {
            ImGui::Text("frametime %f ms", vk_context.stats.frametime);
            ImGui::Text("draw time %f ms", vk_context.stats.mesh_draw_time);
            ImGui::Text("update time %f ms", vk_context.stats.scene_update_time);
            ImGui::Text("triangles %i", vk_context.stats.triangle_count);
            ImGui::Text("draws %i", vk_context.stats.drawcall_count);
            ImGui::End();
        }
        ImGui::End();


        ImGui::Render();

        draw();


        should_kill_game = wi_context.window.should_window_close();

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        vk_context.stats.frametime = elapsed.count() / 1000.f;
    }

}

void flash_bg(VkCommandBuffer cmd, gf::VkManager& context, uint64_t frame_number) {
    VkClearColorValue screen_color;
    float blue_flash = std::abs(std::sin(frame_number / 120.0f));
    screen_color = { {0.0f, blue_flash * 0.5f, blue_flash, 1.0f} };
    VkImageSubresourceRange clear_range = gf::vk_init::subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
    context.draw_background(cmd, screen_color);
}

void gf::Engine::draw() {

    vk_context.update_scene(window_dims.width, window_dims.height); 

    // Synchronization - Wait until frame is ready
    VK_CHECK(vkWaitForFences(vk_context.core.device, 1, &get_current_frame().render_fence, true, 1000000000));
    VK_CHECK(vkResetFences(vk_context.core.device, 1, &get_current_frame().render_fence));

    get_current_frame().deletion_stack.flush();
    get_current_frame().frame_descriptors.clear_pools(vk_context.core.device);

    // Acquire Swapchain - get the swapchain image
    uint32_t swapchain_image_idx;
    VkResult err = vkAcquireNextImageKHR(vk_context.core.device, vk_context.swapchain.swapchain, 1000000000, get_current_frame().swapchain_semaphore, nullptr, &swapchain_image_idx);
    if (err == VK_ERROR_OUT_OF_DATE_KHR) {
        vk_context.resize_requested = true;
        return;
    }

    // Command Buffer - Reset command buffer and get ready for submission
    VkCommandBuffer cmd = get_current_frame().command_buffer;
    VK_CHECK(vkResetCommandBuffer(cmd, 0));
    VkCommandBufferBeginInfo cmd_begin_info = vk_init::begin_command(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VK_CHECK(vkBeginCommandBuffer(cmd, &cmd_begin_info));

    // Image - Set up drawn image
    vk_context.drawn_size.width = std::min(vk_context.swapchain.swapchain_extent.width, vk_context.drawn_image.image_size.width) * vk_context.render_scale;
    vk_context.drawn_size.height = std::min(vk_context.swapchain.swapchain_extent.height, vk_context.drawn_image.image_size.height) * vk_context.render_scale;
    vkl_img::transition_image(cmd, vk_context.drawn_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    
    // Action - perform actual commands
    flash_bg(cmd, vk_context, frame_number); // TEMP FUNCTION - hardcoded with drawn image

    vkl_img::transition_image(cmd, vk_context.drawn_image.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    vkl_img::transition_image(cmd, vk_context.depth_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    vk_context.draw_geometry(cmd, &get_current_frame());

    // Transition and Copy - transition both images to optimal layout and copy image
    vkl_img::transition_image(cmd, vk_context.drawn_image.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkl_img::transition_image(cmd, vk_context.swapchain.swapchain_images[swapchain_image_idx], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    vkl_img::copy_image_to_image(cmd, vk_context.drawn_image.image, vk_context.swapchain.swapchain_images[swapchain_image_idx], vk_context.drawn_size, vk_context.swapchain.swapchain_extent);

    vkl_img::transition_image(cmd, vk_context.swapchain.swapchain_images[swapchain_image_idx], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    // Imgui - Draw imgui
    draw_imgui(cmd, vk_context.swapchain.swapchain_image_views[swapchain_image_idx]);

    // Final Transition - transition transition swapchain image to presentation
    vkl_img::transition_image(cmd, vk_context.swapchain.swapchain_images[swapchain_image_idx], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // Submission and presentation - submit buffer, semaphores, and queue, then present
    VK_CHECK(vkEndCommandBuffer(cmd));
    VkCommandBufferSubmitInfo cmd_info = vk_init::submit_command(cmd);
    VkSemaphoreSubmitInfo wait_info = vk_init::submit_semaphore(get_current_frame().swapchain_semaphore, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);
    VkSemaphoreSubmitInfo signal_info = vk_init::submit_semaphore(get_current_frame().render_semaphore, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);
    VkSubmitInfo2 submit = vk_init::submit_info(&cmd_info, &signal_info, &wait_info);
    VK_CHECK(vkQueueSubmit2(vk_context.core.graphics_queue, 1, &submit, get_current_frame().render_fence));
    VkPresentInfoKHR present_info = vk_init::present_info(&vk_context.swapchain.swapchain, &get_current_frame().render_semaphore, &swapchain_image_idx);
    VkResult present_res = vkQueuePresentKHR(vk_context.core.graphics_queue, &present_info);
    if (present_res == VK_ERROR_OUT_OF_DATE_KHR)
        vk_context.resize_requested = true;

    frame_number++;
}
void gf::Engine::draw_imgui(VkCommandBuffer cmd, VkImageView target_image_view) {
    VkRenderingAttachmentInfo color_attachment = vk_init::attachment_info(target_image_view, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingInfo render_info = vk_init::rendering_info(vk_context.swapchain.swapchain_extent, &color_attachment, nullptr);

    vkCmdBeginRendering(cmd, &render_info);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    vkCmdEndRendering(cmd);
}