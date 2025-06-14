/**
* @file
* @brief Base file for WInput module.
* @author Spencer Banasik
* @details This file is the base WInput (Window and Input) module.
* Owns the GLFW library and serves to generate and modify contexts
* and surfaces.
* @date Created: 1/1/2025
* @date Last Modified: 6/14/2025
*/

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
/**
* @class WInputContext
* @brief A class that bundles and initializes a group of contexts.
* @author Spencer Banasik
* @details This class holds publically accessible context classes with a 
* private constructor that the GLManager uses. This is the main access 
* point for the WInput API.
* @invariant WInputContext owns the glfw user pointer.
* @relates GLManager
*/
class WInputContext {
public:
    friend gf::gl::GLManager;
    WindowContext window;
    MouseContext mouse;
    KeyContext key;
    //JoyContext joystick;

    WInputContext(WInputContext& other) = delete;

    WInputContext(WInputContext&& other) noexcept
        : window(std::move(other.window)), mouse(std::move(other.mouse)), key(std::move(other.key)) {};

private:
    WInputContext(gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr)
        : window(type, window_dims, window_title, monitor), mouse(window_dims, window.window),
        key(window.window) {};
};
/**
* @class GLManager
* @brief GLFW initialization and context creation
* @author Spencer Banasik
* @details Entry point for WInput, creates windows and the surface for windows.
* @invariant WInputContext owns the glfw user pointer.
* @invariant Only GLManager exists.
*/
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
