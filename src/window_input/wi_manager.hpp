/**
* @file
* @brief Base file for WInput module.
* @author Spencer Banasik
* @details This file is the base WInput (Window and Input) module.
* Owns the GLFW library and serves to generate and modify contexts
* and surfaces.
*/

#ifndef WI_MANAGER_HPP
#define WI_MANAGER_HPP

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <wi_types.hpp>
#include <wi_window.hpp>
#include <wi_mouse.hpp>
#include <wi_keys.hpp>

namespace gf {
namespace vk_core {
class VKCore;
}
namespace wi {
/**
* @class WInputContext
* @brief A class that bundles and initializes a group of contexts.
* @author Spencer Banasik
* @details This class holds publically accessible context classes with a 
* private constructor that the GLManager uses. This is the main access 
* point for the WInput API.
* @invariant WInputContext owns the glfw user pointer.
* @relates GLManager
*/
class WInputContext {
public:
    friend gf::wi::WIManager;
    WindowContext window;
    MouseContext mouse;
    KeyContext key;
    //JoyContext joystick;

    /**
    * @brief We don't allow for copy construction
    */
    WInputContext(WInputContext& other) = delete;

    /**
    * @brief Move constructor
    */
    WInputContext(WInputContext&& other) noexcept
        : window(std::move(other.window)), mouse(std::move(other.mouse)), key(std::move(other.key)) {};

private:

    /**
    * @brief Private constructor for GLManager
    * @author Spencer Banasik
    * @private
    */
    WInputContext(wi::WindowType type, wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr)
        : window(type, window_dims, window_title, monitor), mouse(window_dims, window.window),
        key(window.window) {};
};
/**
* @class GLManager
* @brief GLFW initialization and context creation
* @author Spencer Banasik
* @details Entry point for WInput, creates windows and the surface for windows.
* Once the window is created, a surface should be used with the VKCore type to provide 
* a surface to the VulkanAPI.
* @invariant WInputContext owns the glfw user pointer.
* @invariant Only one can GLManager exist.
*/
class WIManager {
public:
    /**
    * @brief Constructor, can be called only once.
    * @author Spencer Banasik
    */
    WIManager();

    /**
    * @brief Destructor that terminates glfw
    * @author Spencer Banasik
    */
    ~WIManager();

    /**
    * @brief We don't allow for copy construction.
    */
    WIManager(const WIManager& other) = delete;

    /**
    * @brief We don't allow for copy assignment.
    */
    WIManager& operator=(const WIManager& other) = delete;

    /**
    * @brief Returns the singleton GLManager
    * @author Spencer Banasik
    * @static
    */
    static WIManager& get();

    /**
    * @brief Create a WInputContext
    * @author Spencer Banasik
    * @details Creates a WInputContext with given parameters. Results in a window, keyboard,
    * and mouse context being created. Sets the window's glfw user pointer to the context.
    * @param [in] type         Whether the window should be borderless, fullscreen, or windowed.
    * @param [in] window_dims  The width and height of the window if it is windowed.
    * @param [in] window_title The title of the window.
    * @param [in] monitor      An optional parameter to select which monitor the window will be on.
    * @returns An initialized WInputContext.
    */
    WInputContext create_window(wi::WindowType type, wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);

    /**
    * @brief Create a surface for Vulkan
    * @author Spencer Banasik
    * @details Uses the WInputContext's window to create a surface and provides it to the VKCore.
    * @param [in, out] gl_context A pointer to a WInputContext.
    * @param [in, out] vk_context A pointer to the VKCore.
    * @returns An initialized WInputContext.
    */
    void create_surface(WInputContext* gl_context, vk_core::VKCore* vk_context);

    /**
    * @brief Check if the GLManager has been initialized
    * @author Spencer Banasik
    */
    bool check_init() { return is_init; };

    /**
    * @brief Temporary function for imgui
    * @author Spencer Banasik
    * @details Gets a window from the WInput context
    */
    GLFWwindow* get_window(WInputContext* gl_context) { return gl_context->window.window; };

private:
    /**@brief Pointer to the singleton variable
    * @static
    */
    static WIManager* loaded_glfw;

    bool is_init = false;
};
}
}
#endif
