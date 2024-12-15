// Spencer Banasik
// Created: 12/14/2024
// Last Modified: 12/14/2024
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

GF::Engine* GF::Engine::loaded_engine = nullptr;

GF::Engine::Engine() {

    assert(loaded_engine == nullptr);
    loaded_engine = this;
    
    if (!glfwInit()) {

        std::cout << "| ERROR: Glfw init failed! Aborting...";
        goto eng_init_error;
    }

    if ((window = init_window(window)) == nullptr) {

        std::cout << "| ERROR: Window wasn't initialized! Aborting...";
        goto eng_init_error;
    }

    // TODO: set callbacks for screen resize
    // TODO: cursor position
    // TODO: and mouse scroll

    is_initialized = true;
    return;

eng_init_error:
    should_kill_game = true;
    return;
}

GF::Engine::~Engine() {
    
    glfwDestroyWindow(window);
    window = nullptr;

    glfwTerminate();
}

GF::Engine& GF::Engine::get() {
    return *loaded_engine;
}

GLFWwindow* GF::Engine::init_window(GLFWwindow* window) {

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: change this later
    window = glfwCreateWindow(window_dims.width, window_dims.height, title.c_str(), nullptr, nullptr);

    return window;
}

void GF::Engine::run() {

    while (should_kill_game == false) {

        // Poll events
        glfwPollEvents();


        if (should_render == false) { // TODO: if the screen is minimized, set to false, else true.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }


        should_kill_game = glfwWindowShouldClose(window);
    }

}

void GF::Engine::draw() {

}