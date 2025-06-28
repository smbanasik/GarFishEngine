// Spencer Banasik
// Created: 1/5/2025
// Last Modified: 1/5/2025
#include <wi_mouse.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <wi_types.hpp>
#include <wi_manager.hpp>

gf::wi::MouseContext::MouseContext(Extent2D window_dims, GLFWwindow* window) 
    : prior_mouse_time(0), current_mouse_time(0), window_handle(window)
{
    mouse_coordinates.x = window_dims.width;
    mouse_coordinates.y = window_dims.height;
    mouse_coordinates.x *= 0.5;
    mouse_coordinates.y *= 0.5;
    prior_mouse_coordinates.x = mouse_coordinates.x;
    prior_mouse_coordinates.y = mouse_coordinates.y;
    current_mouse_time = glfwGetTime();
    prior_mouse_time = current_mouse_time;

    glfwSetCursorPosCallback(window_handle, callback_movement);
}

gf::wi::Double2D gf::wi::MouseContext::get_mouse_coords() {
    return mouse_coordinates;
}
gf::wi::Double2D gf::wi::MouseContext::get_mouse_offset() {
    return { mouse_coordinates.x - prior_mouse_coordinates.x,
        mouse_coordinates.y - prior_mouse_coordinates.y};
}
gf::wi::Double2D gf::wi::MouseContext::get_mouse_velocity() {
    Double2D vector = get_mouse_offset();

    vector.x /= (current_mouse_time - prior_mouse_time);
    vector.y /= (current_mouse_time - prior_mouse_time);

    glm::vec2 vec = glm::normalize(glm::vec2(vector.x, vector.y));
    vector.x = vec.x;
    vector.y = vec.y;

    return vector;
}

void gf::wi::MouseContext::disable_cursor() {
    glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void gf::wi::MouseContext::enable_cursor() {
    glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
void gf::wi::MouseContext::enable_cursor_captured() {
    glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}
void gf::wi::MouseContext::enable_raw_mouse() {
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window_handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}
void gf::wi::MouseContext::disable_raw_mouse() {
    glfwSetInputMode(window_handle, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void gf::wi::MouseContext::callback_movement(GLFWwindow* window, double xpos, double ypos) {
    wi::WInputContext* context = static_cast<wi::WInputContext*>(glfwGetWindowUserPointer(window));

    context->mouse.prior_mouse_coordinates.x = context->mouse.mouse_coordinates.x;
    context->mouse.prior_mouse_coordinates.y = context->mouse.mouse_coordinates.y;
    context->mouse.mouse_coordinates.y = ypos;
    context->mouse.mouse_coordinates.x = xpos;
    context->mouse.prior_mouse_time = context->mouse.current_mouse_time;
    context->mouse.current_mouse_time = glfwGetTime();
    context->mouse.call_movement();
}

void gf::wi::MouseContext::call_movement() {
    if (callback_mouse_move)
        callback_mouse_move(static_cast<wi::WInputContext*>(glfwGetWindowUserPointer(window_handle)));
}