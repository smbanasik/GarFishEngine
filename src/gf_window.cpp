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

gf::GLFWManager* gf::GLFWManager::loaded_glfw = nullptr;

gf::GLFWManager::GLFWManager(uint32_t width, uint32_t height, const std::string& title) {
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
gf::GLFWManager::~GLFWManager() {
    glfwDestroyWindow(window);
    window = nullptr;

    glfwTerminate();
}

gf::GLFWManager& gf::GLFWManager::get() {
    return *loaded_glfw;
}

GLFWwindow* gf::GLFWManager::init_window(GLFWwindow* window, uint32_t width, uint32_t height, const std::string& title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    return window;
}
void gf::GLFWManager::set_user_pointer(void* ptr) {
    glfwSetWindowUserPointer(window, ptr);
}
void gf::GLFWManager::set_callbacak_window_resize(void(*function)(GLFWwindow* window, int width, int height)) {
    glfwSetWindowSizeCallback(window, function);
}
void gf::GLFWManager::set_callback_window_iconified(void(*function)(GLFWwindow* window, int iconified)) {
    glfwSetWindowIconifyCallback(window, function);
}