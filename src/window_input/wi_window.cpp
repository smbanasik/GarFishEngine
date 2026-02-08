// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
#include <window_input/wi_window.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <window_input/wi_types.hpp>
#include <window_input/wi_manager.hpp>

namespace gf {
std::vector<GLFWmonitor*> wi::WindowContext::query_monitors() {
    int count = 0;
    std::vector<GLFWmonitor*> monitors;
    return monitors;
}

wi::WindowContext::WindowContext(wi::WindowType type, wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor)
: type(type), window_dims(window_dims), monitor(monitor) {

    window = create_window(type, window_dims, window_title, monitor);
    glfwSetWindowSizeCallback(window, &callback_size);

}
wi::WindowContext::~WindowContext() {
    if(window != nullptr)
        glfwDestroyWindow(window);
}
wi::WindowContext::WindowContext(WindowContext&& other) noexcept
    : type(std::move(other.type)), window_dims(std::move(other.window_dims)), monitor(std::move(other.monitor)),
    callback_window_resize(std::move(other.callback_window_resize)),
    window(other.window) {;
    other.window = nullptr;
}
void wi::WindowContext::make_fullscreen() {
    glfwSetWindowMonitor(window, (monitor == nullptr) ? glfwGetPrimaryMonitor() : monitor, 0, 0, window_dims.width, window_dims.height, GLFW_DONT_CARE);
}
void wi::WindowContext::make_borderless() {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
}
void wi::WindowContext::make_windowed() {
    glfwSetWindowMonitor(window, nullptr, 0, 0, window_dims.width, window_dims.height, GLFW_DONT_CARE);
}

void wi::WindowContext::callback_size(GLFWwindow* window, int width, int height) {
    wi::WInputContext* context = static_cast<wi::WInputContext*>(glfwGetWindowUserPointer(window));

    context->window.window_dims.width = width;
    context->window.window_dims.height = height;
    context->window.call_resize();
}

GLFWwindow* wi::WindowContext::create_window(gf::wi::WindowType type, wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
    GLFWwindow* window = nullptr;

    switch (type) {
    case wi::WindowType::FULLSCREEN:
        window = helper_create_window_fullscren(window_dims, window_title, monitor);
        break;
    case wi::WindowType::BORDERLESS:
        window = helper_create_window_borderless(window_dims, window_title, monitor);
        break;
    case wi::WindowType::WINDOWED:
    default:
        window = helper_create_window_windowed(window_dims, window_title);
        break;
    }
    return window;
}

GLFWwindow* wi::WindowContext::helper_create_window_windowed(wi::Extent2D window_dims, std::string window_title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(window_dims.width, window_dims.height, window_title.c_str(), nullptr, nullptr);
    return window;
}
GLFWwindow* wi::WindowContext::helper_create_window_fullscren(wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (monitor == nullptr)
        monitor = glfwGetPrimaryMonitor();
    GLFWwindow* window = glfwCreateWindow(window_dims.width, window_dims.height, window_title.c_str(), monitor, nullptr);
    return window;
}
GLFWwindow* wi::WindowContext::helper_create_window_borderless(wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor) {
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
void wi::WindowContext::call_resize() {
    if (callback_window_resize)
        callback_window_resize(static_cast<wi::WInputContext*>(glfwGetWindowUserPointer(window)));
}

}