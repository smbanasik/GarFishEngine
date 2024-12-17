// Spencer Banasik
// Created: 12/14/2024
// Last Modified: 12/16/2024
#include <engine.hpp>

#include <stdint.h>
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>

#include <vk_initializers.hpp>

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

}