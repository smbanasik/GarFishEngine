// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 12/17/2024
#include <gf_window.hpp>

#include <cassert>
#include <iostream>
#include <stdint.h>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

GF::GLFWManager* GF::GLFWManager::loaded_glfw = nullptr;

GF::GLFWManager::GLFWManager(uint32_t width, uint32_t height, const std::string& title) {
    assert(loaded_glfw == nullptr);
    loaded_glfw = this;

    if (!glfwInit()) {
        std::cout << "| ERROR: glfw failed to init. Aborting";
        return;
    }

    if ((window = init_window(window, width, height, title)) == nullptr) {
        std::cout << "| ERROR: Window wasn't initialized! Aborting...";
        return;
    }

    is_init = true;

}
GF::GLFWManager::~GLFWManager() {
    glfwDestroyWindow(window);
    window = nullptr;

    glfwTerminate();
}

GF::GLFWManager& GF::GLFWManager::get() {
    return *loaded_glfw;
}

GLFWwindow* GF::GLFWManager::init_window(GLFWwindow* window, uint32_t width, uint32_t height, const std::string& title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: change this later
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    return window;
}