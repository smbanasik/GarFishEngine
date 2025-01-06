// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/5/2025
// Description:
// Interface for all things related to glfw windows
// gf::gl::GLManager and gf::gl::WInputContext is a friend of this class
// gf::gl::WInputContext owns the user pointer.
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
class WindowContext {
public:
    friend class gf::gl::GLManager;
    friend class gf::gl::WInputContext;
    static std::vector<GLFWmonitor*> query_monitors(); // TODO: implement
    WindowContext(gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);
    ~WindowContext();
    WindowContext(WindowContext& context);
    WindowContext operator=(WindowContext& context) {
        if (this == &context)
            return *this;
        this->window = context.window;
        context.window = nullptr;
        return *this;
    }

    void make_fullscreen();
    void make_borderless();
    void make_windowed();
    void set_monitor(GLFWmonitor* monitor) { this->monitor = monitor; };
    void set_title(std::string title) { glfwSetWindowTitle(window, title.c_str()); };
    void set_callback_window_resize(std::function<void()>&& function) { callback_window_resize = std::move(function); };

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
    std::function<void()> callback_window_resize;

};
}
}
#endif

