/**
* @file
* @brief Window file for WInput module.
* @author Spencer Banasik
* @details This file is the window file for the WInput (Window Input) module.
* Interface for everything related to glfw windows
*/
#ifndef WI_WINDOW_HPP
#define WI_WINDOW_HPP

#include <string>
#include <vector>
#include <functional>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <window_input/wi_types.hpp>

namespace gf {
namespace wi {
class WIManager;
class WInputContext;

/**
* @class WindowContext
* @brief Interface for glfw windows.
* @author Spencer Banasik
* @details API for glfw window creation and manipulation.
* @invariant WInputContext exists.
* @invariant WInputContext owns the glfw user pointer.
* @relates GLManager
* @relates WInputContext
*/
class WindowContext {
public:
    friend class gf::wi::WIManager;
    friend class gf::wi::WInputContext;
    /**
    * @biref @b UNIMPLEMENTED Queries glfw for available monitors
    * @author Spencer Banasik
    * @static
    */
    static std::vector<GLFWmonitor*> query_monitors(); // TODO: implement

    WindowContext(wi::WindowType type, wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);
    
    /**
    * @brief Clears window resource
    * @author Spencer Banasik
    */
    ~WindowContext();

    /**
    * @brief We don't allow copy construction.
    */
    WindowContext(WindowContext& other) = delete;

    /**
    * @brief Move constructor
    * @author Spencer Banasik
    * @details Moves the data from another window context and destroys
    * our current window if it exists.
    * @param [in] other The WindowContext we're cannibalizing.
    */
    WindowContext(WindowContext&& other) noexcept;

    /**
    * @brief We don't allow copy assignment.
    */
    WindowContext operator=(WindowContext& other) = delete;

    /**
    * @brief Move assignment operator
    * @author Spencer Banasik
    * @details Moves the data from another window context and destroys
    * our current window if it exists.
    * @param [in] other The WindowContext we're cannibalizing.
    */
    WindowContext& operator=(WindowContext&& other) noexcept {
        if (this == &other)
            return *this;
        type = std::move(other.type);
        window_dims = std::move(other.window_dims);
        monitor = std::move(other.monitor);
        callback_window_resize = std::move(other.callback_window_resize);
        if (window)
            glfwDestroyWindow(window);
        this->window = other.window;
        other.window = nullptr;
        return *this;
    }

    /**
    * @brief Swap the window to fullscreen
    * @author Spencer Banasik
    */
    void make_fullscreen();
    /**
    * @brief Swap the window to borderless
    * @author Spencer Banasik
    */
    void make_borderless();
    /**
    * @brief Swap the window to windowed
    * @author Spencer Banasik
    */
    void make_windowed();
    /**
    * @brief Set the monitor of the screen
    * @author Spencer Banasik
    * @param [in] monitor The queried monitor to be used.
    */
    void set_monitor(GLFWmonitor* monitor) { this->monitor = monitor; };
    /**
    * @brief Set the title of the window
    * @author Spencer Banasik
    * @param [in] title The new title of the window.
    */
    void set_title(std::string title) { glfwSetWindowTitle(window, title.c_str()); };
    /**
    * @brief Set the callback function for when the window is resized
    * @author Spencer Banasik
    * @details GLFW allows for a callback function to be set when the user
    * resizes the window. This function allows for a function to be included
    * in the default WInput resize callback.
    * @param [in] function A pointer to the function.
    */
    void set_callback_window_resize(std::function<void(WInputContext* wi_context)>&& function) { callback_window_resize = std::move(function); };

    /**
    * @brief Determines whether the window is done being used.
    * @author Spencer Banasik
    * @returns Whether the window should be closed or not.
    */
    bool should_window_close() { return glfwWindowShouldClose(window); };
    
    /**
    * @brief Get the dimensions of a window
    * @author Spencer Banasik
    * @returns The dimensions of the window
    */
    wi::Extent2D get_window_dims() { return window_dims; };

private:

    /**
    * @brief The default callback function for resizing
    * @author Spencer Banasik
    * @details Every window's resize callback will be this function. It uses the 
    * window's user pointer to gain access to the context, then sets the context's
    * window dimensions, and finally calls the user added callback for additional
    * functionality.
    * @private
    */
    static void callback_size(GLFWwindow* window, int width, int height);

    /**
    * @brief Make appropriate library call according to WindowType
    * @author Spencer Banasik
    * @private
    */
    GLFWwindow* create_window(wi::WindowType type, wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);

    /**
    * @brief Make library call to create a windowed window
    * @author Spencer Banasik
    * @private
    */
    GLFWwindow* helper_create_window_windowed(wi::Extent2D window_dims, std::string window_title);

    /**
    * @brief Make library call to create a fullscreen window
    * @author Spencer Banasik
    * @private
    */
    GLFWwindow* helper_create_window_fullscren(wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);

    /**
    * @brief Make library call to create a borderless window
    * @author Spencer Banasik
    * @private
    */
    GLFWwindow* helper_create_window_borderless(wi::Extent2D window_dims, std::string window_title, GLFWmonitor* monitor = nullptr);

    /**
    * @brief Set the window's user pointer
    * @author Spencer Banasik
    * @private
    */
    void set_user_pointer(void* p_user) { glfwSetWindowUserPointer(window, p_user); };

    /**
    * @brief Call the callback_window_resize if it exists
    * @author Spencer Banasik
    * @private
    */
    void call_resize();

    WindowType type;
    wi::Extent2D window_dims;

    GLFWwindow* window;
    GLFWmonitor* monitor;
    std::function<void(WInputContext* wi_context)> callback_window_resize;

};
}
}
#endif

