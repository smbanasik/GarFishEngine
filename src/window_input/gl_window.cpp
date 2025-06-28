// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
#include <gl_window.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <gl_types.hpp>
#include <gl_manager.hpp>

namespace gf {
std::vector<GLFWmonitor*> gl::WindowContext::query_monitors() {
    int count = 0;
    std::vector<GLFWmonitor*> monitors;
    return monitors;
}

gl::WindowContext::WindowContext(gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor)
: type(type), window_dims(window_dims), monitor(monitor) {

    window = create_window(type, window_dims, window_title, monitor);
    glfwSetWindowSizeCallback(window, &callback_size);

}
gl::WindowContext::~WindowContext() {
    if(window != nullptr)
        glfwDestroyWindow(window);
}
gl::WindowContext::WindowContext(WindowContext&& other) noexcept
    : type(std::move(other.type)), window_dims(std::move(other.window_dims)), monitor(std::move(other.monitor)),
    callback_window_resize(std::move(other.callback_window_resize)),
    window(other.window) {;
    other.window = nullptr;
}
void gl::WindowContext::make_fullscreen() {
    glfwSetWindowMonitor(window, (monitor == nullptr) ? glfwGetPrimaryMonitor() : monitor, 0, 0, window_dims.width, window_dims.height, GLFW_DONT_CARE);
}
void gl::WindowContext::make_borderless() {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}
void gl::WindowContext::make_windowed() {
    glfwSetWindowMonitor(window, nullptr, 0, 0, window_dims.width, window_dims.height, GLFW_DONT_CARE);
}

void gl::WindowContext::callback_size(GLFWwindow* window, int width, int height) {
    gl::WInputContext* context = static_cast<gl::WInputContext*>(glfwGetWindowUserPointer(window));

    context->window.window_dims.width = width;
    context->window.window_dims.height = height;
    context->window.call_resize();
}

GLFWwindow* gl::WindowContext::create_window(gf::gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
    GLFWwindow* window = nullptr;

    switch (type) {
    case gl::WindowType::FULLSCREEN:
        window = helper_create_window_fullscren(window_dims, window_title, monitor);
        break;
    case gl::WindowType::BORDERLESS:
        window = helper_create_window_borderless(window_dims, window_title, monitor);
        break;
    case gl::WindowType::WINDOWED:
    default:
        window = helper_create_window_windowed(window_dims, window_title);
        break;
    }
    return window;
}

GLFWwindow* gl::WindowContext::helper_create_window_windowed(gl::Extent2D window_dims, std::string window_title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(window_dims.width, window_dims.height, window_title.c_str(), nullptr, nullptr);
    return window;
}
GLFWwindow* gl::WindowContext::helper_create_window_fullscren(gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (monitor == nullptr)
        monitor = glfwGetPrimaryMonitor();
    GLFWwindow* window = glfwCreateWindow(window_dims.width, window_dims.height, window_title.c_str(), monitor, nullptr);
    return window;
}
GLFWwindow* gl::WindowContext::helper_create_window_borderless(gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
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
void gl::WindowContext::call_resize() {
    if (callback_window_resize)
        callback_window_resize(static_cast<gl::WInputContext*>(glfwGetWindowUserPointer(window)));
}

}