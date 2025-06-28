// Spencer Banasik
// Created: 1/06/2025
// Last Modified: 1/06/2025
#include <gf_err.hpp>

#include <iostream>

#include <GLFW/glfw3.h>

using namespace gf;

void err::vk::VulkanError::display() {
    std::cout << "G-VK| ERROR: " << what() << "\n";
}
const char* err::vk::VulkanError::what() {
    return "An error in vulkan has occured.";
}
void err::vk::VulkanError::display(const char* string_vkresult) {
    std::cout << "G-VK| ERROR: " << string_vkresult << "\n";
}

void err::gl::GLFWError::display() {
    std::cout << "G-GL| ERROR: " << what() << "\n";
}

const char* err::gl::GLFWError::what() {
    const char* desc;
    glfwGetError(&desc);
    return desc;
}

const char* err::gl::GLFWInitErr::what() {
    return "GLFW Initialization has failed!";
}