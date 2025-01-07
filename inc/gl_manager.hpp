// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/5/2025
// Description:
// Owns glfw library
// Serves to generate and modify contexts
// As well as surfaces
#ifndef GL_MANAGER_HPP
#define GL_MANAGER_HPP

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <gl_types.hpp>
#include <gl_window.hpp>
#include <gl_mouse.hpp>
#include <gl_keys.hpp>

namespace gf {
namespace vk_core {
class VKCore;
}
namespace gl {
class WInputContext {
public:
    friend gf::gl::GLManager;
    WindowContext window;
    MouseContext mouse;
    KeyContext key;
    //JoyContext joystick;

    WInputContext(WInputContext& other)
        : window(other.window), mouse(other.mouse), key(other.key) {};

    WInputContext(WInputContext&& other) noexcept
        : window(std::move(other.window)), mouse(std::move(other.mouse)), key(std::move(other.key)) {};

private:
    WInputContext(gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr)
        : window(type, window_dims, window_title, monitor), mouse(window_dims, window.window),
        key(window.window) {};
};

class GLManager {
public:
    GLManager();
    ~GLManager();
    GLManager& get();

    WInputContext create_window(gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);

    void create_surface(WInputContext* gl_context, vk_core::VKCore* vk_context);

    bool check_init() { return is_init; };

    // Temporary function for imgui
    GLFWwindow* get_window(WInputContext* gl_context) { return gl_context->window.window; };

private:
    static GLManager* loaded_glfw;
    GLManager(const GLManager& other) = delete;
    GLManager& operator=(const GLManager& other) = delete;

    bool is_init = false;
};
}
}
#endif
