// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
// Description:
// Types for glfw files
#ifndef GL_TYPES_HPP
#define GL_TYPES_HPP

#include <stdint.h>
#include <string>

struct GLFWwindow;
struct GLFWmonitor;

namespace gf {
namespace gl {
struct Extent2D {
    uint32_t width;
    uint32_t height;
};
struct Float2D {
    float x;
    float y;
};
struct Double2D {
    double x;
    double y
};

enum class WindowType {
    WINDOWED,
    FULLSCREEN,
    BORDERLESS
};

}
}
#endif
