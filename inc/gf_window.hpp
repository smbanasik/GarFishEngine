// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 12/17/2024
// Description:
// To serve as an interface for the window and GLFW
// GLFWManager owns GLFW library and initialization.
#ifndef GF_WINDOW_HPP
#define GF_WINDOW_HPP

struct GLFWwindow;
struct VkExtent2D;

#include <stdint.h>
#include <string>

namespace GF {
// The GLFWManager owns all GLFW resources and the library
class GLFWManager {
public:
    GLFWwindow* window = nullptr;
    bool is_init = false;

    GLFWManager(uint32_t width, uint32_t height, const std::string& title);
    ~GLFWManager();

    GLFWManager& get();

private:
    static GLFWManager* loaded_glfw;
    GLFWManager(const GLFWManager& other) = delete;
    GLFWManager& operator=(const GLFWManager& other) = delete;
    GLFWwindow* init_window(GLFWwindow* window, uint32_t width, uint32_t height, const std::string& title);
};
}
#endif