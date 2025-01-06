// Spencer Banasik
// Created: 1/5/2025
// Last Modified: 1/5/2025
// Description:
// Interface for all things related to glfw mice
// gf::GLManager is a friend of this class
#ifndef GL_MOUSE_HPP
#define GL_MOUSE_HPP

#include <functional>

#include <gl_types.hpp>

struct GLFWwindow;

namespace gf {
namespace gl {
class GLManager;
class MouseContext {
    friend class gf::gl::GLManager;
public:

    MouseContext(Extent2D window_dims, GLFWwindow* window);

    Double2D get_mouse_coords();
    Double2D get_mouse_offset();
    Double2D get_mouse_velocity();

    void set_callback_mouse_movement(std::function<void()>&& function) { callback_mouse_move = std::move(function); };

    void disable_cursor();
    void enable_cursor();
    void enable_cursor_captured();
    void enable_raw_mouse();
    void disable_raw_mouse();

    bool first_mouse = true;

private:

    static void callback_movement(GLFWwindow* window, double xpos, double ypos);

    void call_movement();

    double prior_mouse_time;
    double current_mouse_time;
    Double2D mouse_coordinates;
    Double2D prior_mouse_coordinates;
    std::function<void()> callback_mouse_move;
    GLFWwindow* window_handle = nullptr;
};
}
}

#endif