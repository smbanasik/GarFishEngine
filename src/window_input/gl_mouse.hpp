/**
* @file
* @brief File that contains mouse WInput module.
* @author Spencer Banasik
* @date Created: 1/1/2025
* @date Last Modified: 6/16/2025
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
* @pre The glfw library must be initialized.
*/
class MouseContext {
    friend class gf::gl::GLManager;
public:

    /**
    * @brief Constructor, sets mouse movement callback.
    * @author Spencer Banasik
    */
    MouseContext(Extent2D window_dims, GLFWwindow* window);

    /**
    * @brief Gets mouse coordinates.
    * @author Spencer Banasik
    * @returns The current coordinates of the mouse.
    */
    Double2D get_mouse_coords();

    /**
    * @brief Gets the offset of the mouse with respect to to it's last coordinates.
    * @author Spencer Banasik
    * @returns The offset of the mouse.
    */
    Double2D get_mouse_offset();

    /**
    * @brief Gets the velocity of it's mouse with respect to it's last coordinates.
    * @author Spencer Banasik
    * @details Divides the mouse offset by the time difference of the current and previous coordinates.
    * Vector is then normalized.
    * @returns The velocity of the mouse.
    */
    Double2D get_mouse_velocity();


    /**
    * @brief Allows the user to add their own functionality to the mouse movement callback.
    * @author Spencer Banasik
    */
    void set_callback_mouse_movement(std::function<void(WInputContext* gl_context)>&& function) { callback_mouse_move = std::move(function); };

    /**
    * @brief Disables the mouse cursor, used for 3D camera.
    * @author Spencer Banasik
    */
    void disable_cursor();

    /**
    * @brief Enables the mouse cursor.
    * @author Spencer Banasik
    */
    void enable_cursor();

    /**
    * @brief Mouse cursor is restricted to the window?
    * @author Spencer Banasik
    */
    void enable_cursor_captured();

    /**
    * @brief Enables raw (unscaled and unaccelerated) mouse motion.
    * @author Spencer Banasik
    */
    void enable_raw_mouse();

    /**
    * @brief Disables raw mouse motion.
    * @author Spencer Banasik
    */
    void disable_raw_mouse();

    bool first_mouse = true;

private:

    /**
    * @brief Callback for the glfw mouce movement.
    * @author Spencer Banasik
    * @details Sets the mouse times and mouse coordinates.
    * @private
    */
    static void callback_movement(GLFWwindow* window, double xpos, double ypos);

    /**
    * @brief Calls the user added mouse movement callback.
    * @author Spencer Banasik
    * @private
    */
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