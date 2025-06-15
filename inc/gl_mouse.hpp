/**
* @file
* @brief File that contains mouse WInput module.
* @author Spencer Banasik
* @date Created: 1/1/2025
* @date Last Modified: 6/15/2025
*/
#ifndef GL_MOUSE_HPP
#define GL_MOUSE_HPP

#include <functional>

#include <gl_types.hpp>

struct GLFWwindow;

namespace gf {
namespace gl {
class GLManager;
class WInputContext;
/**
* @class GLManager
* @brief GLFW initialization and context creation
* @author Spencer Banasik
* @details WInput mouse API, calculates items like mouse coordinates, offset,
* and velocity. Allows for callbacks to occur on mouse movement.
* Used to modify elements of cursor.
* @invariant Requires glfw library during usage.
*/
class MouseContext {
    friend class gf::gl::GLManager;
public:

    MouseContext(Extent2D window_dims, GLFWwindow* window);

    Double2D get_mouse_coords();
    Double2D get_mouse_offset();
    Double2D get_mouse_velocity();

    void set_callback_mouse_movement(std::function<void(WInputContext* gl_context)>&& function) { callback_mouse_move = std::move(function); };

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
    std::function<void(WInputContext* gl_context)> callback_mouse_move;
    GLFWwindow* window_handle = nullptr;
};
}
}

#endif