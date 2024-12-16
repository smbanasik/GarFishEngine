// Spencer Banasik
// Created: 12/14/2024
// Last Modified: 12/16/2024
// Description:
// This file serves as the driver for window creation and the main engine pipeline
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

GF::Engine* GF::Engine::loaded_engine = nullptr;

GF::Engine::Engine() : gl_context(window_dims, title), vk_context(gl_context.window, window_dims.width, window_dims.height) {

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

}