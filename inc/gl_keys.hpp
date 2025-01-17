// Spencer Banasik
// Created: 1/6/2025
// Last Modified: 1/6/2025
// Description:
// Interface for all things related to glfw keys and chars
// gf::gl::GLManager is a friend of this class
// gf::gl::WInputContext owns the user pointer.
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
class KeyContext {
    friend class gf::gl::GLManager;
public:

    KeyContext(GLFWwindow* window);

    void set_key_mapping(int key, std::function<void(gl::WInputContext* context, Key* key)>&& function);
    void set_callback_keyboard_key(std::function<void(WInputContext* gl_context)>&& function) { callback_keyboard_key = std::move(function); };
    void set_callback_keyboard_char(std::function<void(WInputContext* gl_context)>&& function) { callback_keyboard_char = std::move(function); };
    const std::wstring& get_char_buffer() { return char_buffer; };
    void clear_char_buffer() { char_buffer.clear(); };

private:

    static void callback_key(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void callback_char(GLFWwindow* window, unsigned int codepoint);

    void call_key();
    void call_char();
    void set_window_handle(GLFWwindow* window_handle) { this->window_handle = window_handle; };

    std::function<void(WInputContext* gl_context)> callback_keyboard_char;
    std::function<void(WInputContext* gl_context)> callback_keyboard_key;
    GLFWwindow* window_handle = nullptr;
    std::unordered_map <int, std::function<void(gl::WInputContext* context, Key* key)>> key_map;
    std::wstring char_buffer;

};
}
}
#endif