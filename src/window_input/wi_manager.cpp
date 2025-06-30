// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
#include <wi_manager.hpp>

#include <cassert>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <vkl_core.hpp>

gf::wi::WIManager* gf::wi::WIManager::loaded_glfw = nullptr;

gf::wi::WIManager::WIManager() {
    assert(loaded_glfw == nullptr);
    loaded_glfw = this;
    
    if (!glfwInit()) {
        throw;
    }

    is_init = true;
}
gf::wi::WIManager::~WIManager() {
    glfwTerminate();
}
gf::wi::WIManager& gf::wi::WIManager::get() {
    return *loaded_glfw;
}

gf::wi::WInputContext gf::wi::WIManager::create_window(wi::WindowType type, wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
    WInputContext context(type, window_dims, window_title, monitor);
    context.window.set_user_pointer(&context);
    return context;
}

void gf::wi::WIManager::create_surface(WInputContext* gl_context, vkl_core::VKCore* vk_context) {
    glfwCreateWindowSurface(vk_context->instance, gl_context->window.window, NULL, &vk_context->surface);
}