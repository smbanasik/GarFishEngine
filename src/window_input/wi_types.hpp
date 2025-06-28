/**
* @file
* @brief File that contains types used in WInput module.
* @author Spencer Banasik
*/
#ifndef WI_TYPES_HPP
#define WI_TYPES_HPP

#include <stdint.h>

namespace gf {
namespace wi {

/**
* @struct Extent2D
* @brief Simple struct that contains a width and height.
* @author Spencer Banasik
* @details This is based off of Vulkan's Extent2D, but with the
* benefit that it is blind to the Vulkan library. This allows 
* the WInput module to be slightly less reliant on Vulkan, 
* though the GLManager still requires Vulkan.
*/
struct Extent2D {
    uint32_t width;
    uint32_t height;
};

/**
* @struct Float2D
* @brief A coordinate with float precision.
* @author Spencer Banasik
*/
struct Float2D {
    float x;
    float y;
};
/**
* @struct Double2D
* @brief A coordinate with double precision.
* @author Spencer Banasik
*/
struct Double2D {
    double x;
    double y;
};

/**
* @enum WindowType
* @brief Enumerator for types of glfw windows.
* @author Spencer Banasik
*/
enum class WindowType {
    WINDOWED,
    FULLSCREEN,
    BORDERLESS
};

/**
* @struct Key
* @brief Structure for keyboard input, containing the key itself, the action of the keyboard, and any modifiers (shift, ctrl, etc.).
* @author Spencer Banasik
*/
struct Key {
    int key;
    int action;
    int modifiers;
};

}
}
#endif
