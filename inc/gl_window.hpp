/**
* @file
* @brief Window file for WInput module.
* @author Spencer Banasik
* @details This file is the window file for the WInput (Window Input) module.
* Interface for everything related to glfw windows
* @date Created: 1/1/2025
* @date Last Modified: 6/14/2025
*/
#ifndef GL_WINDOW_HPP
#define GL_WINDOW_HPP

#include <string>
#include <vector>
#include <functional>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <gl_types.hpp>

namespace gf {
namespace gl {
class GLManager;
class WInputContext;

/**
* @class WindowContext
* @brief Interface for glfw windows.
* @author Spencer Banasik
* @details API for glfw window creation and manipulation.
* @invariant WInputContext exists.
* @invariant WInputContext owns the glfw user pointer.
* @relates GLManager
* @relates WInputContext
*/
class WindowContext {
public:
    friend class gf::gl::GLManager;
    friend class gf::gl::WInputContext;
    static std::vector<GLFWmonitor*> query_monitors(); // TODO: implement
    WindowContext(gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);
    ~WindowContext();
    WindowContext(WindowContext& other) = delete;
    WindowContext(WindowContext&& other) noexcept;
    WindowContext operator=(WindowContext& other) = delete;
    WindowContext& operator=(WindowContext&& other) noexcept {
        if (this == &other)
            return *this;
        type = std::move(other.type);
        window_dims = std::move(other.window_dims);
        monitor = std::move(other.monitor);
        callback_window_resize = std::move(other.callback_window_resize);
        if (window)
            glfwDestroyWindow(window);
        this->window = other.window;
        other.window = nullptr;
        return *this;
    }

    void make_fullscreen();
    void make_borderless();
    void make_windowed();
    void set_monitor(GLFWmonitor* monitor) { this->monitor = monitor; };
    void set_title(std::string title) { glfwSetWindowTitle(window, title.c_str()); };
    void set_callback_window_resize(std::function<void(WInputContext* gl_context)>&& function) { callback_window_resize = std::move(function); };

    bool should_window_close() { return glfwWindowShouldClose(window); };
    
    gl::Extent2D get_window_dims() { return window_dims; };

private:

    static void callback_size(GLFWwindow* window, int width, int height);

    GLFWwindow* create_window(gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);
    GLFWwindow* helper_create_window_windowed(gl::Extent2D window_dims, std::string window_title);
    GLFWwindow* helper_create_window_fullscren(gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);
    GLFWwindow* helper_create_window_borderless(gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);

    void set_user_pointer(void* p_user) { glfwSetWindowUserPointer(window, p_user); };
    void call_resize();

    WindowType type;
    gl::Extent2D window_dims;

    GLFWwindow* window;
    GLFWmonitor* monitor;
    std::function<void(WInputContext* gl_context)> callback_window_resize;

};
}
}
#endif

