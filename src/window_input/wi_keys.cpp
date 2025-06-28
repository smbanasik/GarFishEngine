// Spencer Banasik
// Created: 1/6/2025
// Last Modified: 1/6/2025
#include <wi_keys.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <wi_types.hpp>
#include <wi_manager.hpp>

gf::wi::KeyContext::KeyContext(GLFWwindow* window)
    : window_handle(window), char_buffer(L"") {
    glfwSetKeyCallback(window_handle, callback_key);
    glfwSetCharCallback(window_handle, callback_char);
}

void gf::wi::KeyContext::set_key_mapping(int key, std::function<void(wi::WInputContext* context, Key*)>&& function) {
    key_map.insert(std::make_pair(key, function));
}

void gf::wi::KeyContext::callback_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    wi::WInputContext* context = static_cast<wi::WInputContext*>(glfwGetWindowUserPointer(window));
    Key new_key{ key, action, mods };
    if (context->key.key_map.contains(key)) {
        context->key.key_map[key](context, &new_key);
    }
    context->key.call_key(&new_key);
}
void gf::wi::KeyContext::callback_char(GLFWwindow* window, unsigned int codepoint) {
    wi::WInputContext* context = static_cast<wi::WInputContext*>(glfwGetWindowUserPointer(window));
    context->key.char_buffer += std::to_wstring(codepoint);
    context->key.call_char();
}

void gf::wi::KeyContext::call_key(Key* key) {
    if (callback_keyboard_key)
        callback_keyboard_key(static_cast<wi::WInputContext*>(glfwGetWindowUserPointer(window_handle)), key);

}
void gf::wi::KeyContext::call_char() {
    if (callback_keyboard_char)
        callback_keyboard_char(static_cast<wi::WInputContext*>(glfwGetWindowUserPointer(window_handle)));
}