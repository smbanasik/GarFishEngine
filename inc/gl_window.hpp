// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
// Description:
// Interface for all things related to glfw windows
#ifndef GL_WINDOW_HPP
#define GL_WINDOW_HPP

#include <string>
#include <vector>

#include <gl_types.hpp>

struct GLFWwindow;
struct GLFWmonitor;

namespace gf {
namespace gl_window {

// Optional, for memory safety
class SafeWindow {
public:
    GLFWwindow* window = nullptr;
    ~SafeWindow();
};

GLFWwindow* create_window(gl::WindowType type, gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);
void destroy_window(GLFWwindow* window);

void set_window_type(GLFWwindow* window, gl::WindowType type);
void set_minimum_window_dims(GLFWwindow* window, gl::Extent2D min_dims);
void set_window_dims(GLFWwindow* window, gl::Extent2D window_dims);
void set_window_title(GLFWwindow* window, std::string title);
void make_fullscreen(GLFWwindow* window, GLFWmonitor* display_monitor);
void make_borderless(GLFWwindow* window, GLFWmonitor* display_monitor);
void make_windowed(GLFWwindow* window);

namespace helper {
GLFWwindow* helper_create_window_windowed(gl::Extent2D window_dims, std::string window_title);
GLFWwindow* helper_create_window_fullscren(gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);
GLFWwindow* helper_create_window_borderless(gl::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);
std::vector<GLFWmonitor*> helper_query_monitors();
}

}
}
#endif

