// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
#include <gl_window.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <gl_types.hpp>
namespace gf {
gl_window::SafeWindow::~SafeWindow() {
    if (window != nullptr)
        destroy_window(window);
}

GLFWwindow* gl_window::create_window(gf::gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
    GLFWwindow* window = nullptr;

    switch (type) {
    case gl::WindowType::FULLSCREEN:
        helper::helper_create_window_fullscren(window_dims, window_title, monitor);
        break;
    case gl::WindowType::BORDERLESS:
        helper::helper_create_window_borderless(window_dims, window_title, monitor);
        break;
    case gl::WindowType::WINDOWED:
    default:
        helper::helper_create_window_windowed(window_dims, window_title);
        break;
    }

    return window;
}
void gl_window::destroy_window(GLFWwindow* window) {
    glfwDestroyWindow(window);
}

void gl_window::set_window_type(GLFWwindow* window, gl::WindowType type) {
    return;
}
void gl_window::set_window_dims(GLFWwindow* window, gl::Extent2D window_dims) {
    return;
}
void gl_window::set_window_title(GLFWwindow* window, std::string title) {
    return;
}

GLFWwindow* gl_window::helper::helper_create_window_windowed(gl::Extent2D window_dims, std::string window_title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(window_dims.width, window_dims.height, window_title.c_str(), nullptr, nullptr);
    return window;
}
GLFWwindow* gl_window::helper::helper_create_window_fullscren(gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (monitor == nullptr)
        monitor = glfwGetPrimaryMonitor();
    GLFWwindow* window = glfwCreateWindow(window_dims.width, window_dims.height, window_title.c_str(), monitor, nullptr);
    return window;
}
GLFWwindow* gl_window::helper::helper_create_window_borderless(gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (monitor == nullptr)
        monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "My Title", monitor, nullptr);

    return nullptr;
}

}