// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
#include <gl_manager.hpp>

#include <cassert>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <vk_core.hpp>
#define GF_VK_CHECK_TRUE
#include <gf_err.hpp>

gf::gl::GLManager* gf::gl::GLManager::loaded_glfw = nullptr;

gf::gl::GLManager::GLManager() {
    assert(loaded_glfw == nullptr);
    loaded_glfw = this;
    
    if (!glfwInit()) {
        err::gl::GLFWInitErr err;
        err.display();
        throw err;
    }
}
gf::gl::GLManager::~GLManager() {
    glfwTerminate();
}
gf::gl::GLManager& gf::gl::GLManager::get() {
    return *this;
}

gf::gl::WInputContext gf::gl::GLManager::create_window(gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
    WInputContext context(type, window_dims, window_title, monitor);
    context.window.set_user_pointer(&context);
    return context;
}

void gf::gl::GLManager::create_surface(WInputContext* gl_context, vk_core::VKCore* vk_context) {
    VK_CHECK(glfwCreateWindowSurface(vk_context->instance, gl_context->window.window, NULL, &vk_context->surface));
}