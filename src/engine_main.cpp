// Spencer Banasik
// Created: 12/14/2024
// Last Modified: 12/16/2024
#include <engine.hpp>

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

#include <vk_initializers.hpp>
#include <vk_images.hpp>

GF::Engine* GF::Engine::loaded_engine = nullptr;

GF::Engine::Engine() : gl_context(window_dims.width, window_dims.height, title), vk_context(gl_context.window, window_dims.width, window_dims.height) {

    assert(loaded_engine == nullptr);
    loaded_engine = this;

    // TODO: set callbacks for screen resize
    // TODO: cursor position
    // TODO: and mouse scroll
}

GF::Engine::~Engine() {
}

GF::Engine& GF::Engine::get() {
    return *loaded_engine;
}

void GF::Engine::run() {

    while (should_kill_game == false) {

        // Poll events
        glfwPollEvents();


        if (should_render == false) { // TODO: if the screen is minimized, set to false, else true.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        draw();


        should_kill_game = glfwWindowShouldClose(gl_context.window);
    }

}

void GF::Engine::draw() {

    vkWaitForFences(vk_context.device, 1, &get_current_frame().render_fence, true, 1000000000);
    vkResetFences(vk_context.device, 1, &get_current_frame().render_fence);

    uint32_t swapchain_image_idx;
    vkAcquireNextImageKHR(vk_context.device, vk_context.swapchain.swapchain, 1000000000, get_current_frame().swapchain_semaphore, nullptr, &swapchain_image_idx);

    VkCommandBuffer cmd = get_current_frame().command_buffer;
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo cmd_begin_info = init_vk_begin_command(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkBeginCommandBuffer(cmd, &cmd_begin_info);

    transition_image(cmd, vk_context.swapchain.swapchain_images[swapchain_image_idx], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    VkClearColorValue screen_color;
    float blue_flash = std::abs(std::sin(frame_number / 120.0f));
    screen_color = { {0.0f, 0.1f, blue_flash, 1.0f} };
    VkImageSubresourceRange clear_range = init_vk_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
    vkCmdClearColorImage(cmd, vk_context.swapchain.swapchain_images[swapchain_image_idx], VK_IMAGE_LAYOUT_GENERAL, &screen_color, 1, &clear_range);
    transition_image(cmd, vk_context.swapchain.swapchain_images[swapchain_image_idx], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    vkEndCommandBuffer(cmd);

    VkCommandBufferSubmitInfo cmd_info = init_vk_submit_command(cmd);
    VkSemaphoreSubmitInfo wait_info = init_vk_submit_semaphore(get_current_frame().swapchain_semaphore, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);
    VkSemaphoreSubmitInfo signal_info = init_vk_submit_semaphore(get_current_frame().render_semaphore, VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);
    VkSubmitInfo2 submit = init_vk_submit_info(&cmd_info, &signal_info, &wait_info);

    vkQueueSubmit2(vk_context.graphics_queue, 1, &submit, get_current_frame().render_fence);

    // TODO: move this to info function
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.pSwapchains = &vk_context.swapchain.swapchain;
    present_info.swapchainCount = 1;
    present_info.pWaitSemaphores = &get_current_frame().render_semaphore;
    present_info.waitSemaphoreCount = 1;
    present_info.pImageIndices = &swapchain_image_idx;

    vkQueuePresentKHR(vk_context.graphics_queue, &present_info);

    frame_number++;
}