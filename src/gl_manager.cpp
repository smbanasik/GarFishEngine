// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
#include <gl_manager.hpp>

#include <cassert>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

gf::gl::GLManager* gf::gl::GLManager::loaded_glfw = nullptr;

gf::gl::GLManager::GLManager() {
    assert(loaded_glfw == nullptr);
    loaded_glfw = this;
    
    glfwInit();
}
gf::gl::GLManager::~GLManager() {
    glfwTerminate();
}
gf::gl::GLManager& gf::gl::GLManager::get() {
    return *this;
}