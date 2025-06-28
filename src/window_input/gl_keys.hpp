/**
* @file
* @brief File that contains keyboard WInput module.
* @author Spencer Banasik
* @date Created: 1/1/2025
* @date Last Modified: 6/16/2025
*/
#ifndef GL_KEYS_HPP
#define GL_KEYS_HPP

#include <functional>
#include <unordered_map>
#include <string>

struct GLFWwindow;

namespace gf {
namespace gl {
class GLManager;
class WInputContext;
struct Key;
/**
* @class KeyContext
* @brief Keyboard API for WInput Module
* @author Spencer Banasik
* @details @b TODO!!!!! 
* a surface to the VulkanAPI.
*/
class KeyContext {
    friend class gf::gl::GLManager;
public:

    KeyContext(GLFWwindow* window);

    /**
    * @brief Sets a mapping between a key having an action occur and a user specific function.
    * @author Spencer Banasik
    */
    void set_key_mapping(int key, std::function<void(gl::WInputContext* context, Key* key)>&& function);

    /**
    * @brief Allows the user to add their own functionality to the key action callback, which will always be called.
    * @author Spencer Banasik
    */
    void set_callback_keyboard_key(std::function<void(WInputContext* gl_context, Key* key)>&& function) { callback_keyboard_key = std::move(function); };
    
    /**
    * @brief Allows the user to add their own functionality to the character callback.
    * @author Spencer Banasik
    */
    void set_callback_keyboard_char(std::function<void(WInputContext* gl_context)>&& function) { callback_keyboard_char = std::move(function); };
    
    /**
    * @brief Provides a constant reference to the char buffer.
    * @author Spencer Banasik
    */
    const std::wstring& get_char_buffer() { return char_buffer; };

    /**
    * @brief Empties the character buffer.
    * @author Spencer Banasik
    */
    void clear_char_buffer() { char_buffer.clear(); };

private:

    /**
    * @brief Callback for the glfw key pressed.
    * @author Spencer Banasik
    * @details If the key map contains the key, assemble it into the key struct,
    * and then call the associated function in the keymap with the new key and the context.
    * Finally, call the user added key function.
    * 
    * The idea behind this design is that the user can map behavior to each key, and optionally call
    * code after each key, such as if they want to store their own keys.
    * @private
    */
    static void callback_key(GLFWwindow* window, int key, int scancode, int action, int mods);

    /**
    * @brief Callback for the glfw char pressed.
    * @author Spencer Banasik
    * @details Converts the character into a string and then adds it to the character buffer, then calls
    * the user added callback.
    * @private
    */
    static void callback_char(GLFWwindow* window, unsigned int codepoint);

    /**
    * @brief Calls the user added key callback.
    * @author Spencer Banasik
    * @private
    * @param [in] A non owning pointer to the key assembled in callback_key.
    */
    void call_key(Key* key);

    /**
    * @brief Calls the user added char callback.
    * @author Spencer Banasik
    * @private
    */
    void call_char();

    /**
    * @brief Provide a handle to the window.
    * @author Spencer Banasik
    * @note Do we need this?
    * @private
    */
    void set_window_handle(GLFWwindow* window_handle) { this->window_handle = window_handle; };

    std::function<void(WInputContext* gl_context)> callback_keyboard_char;
    std::function<void(WInputContext* gl_context, Key* key)> callback_keyboard_key;
    GLFWwindow* window_handle = nullptr;
    std::unordered_map <int, std::function<void(gl::WInputContext* context, Key* key)>> key_map;
    std::wstring char_buffer;

};
}
}
#endif