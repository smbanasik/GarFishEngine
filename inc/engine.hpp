/**
* @file
* @brief Main engine header file.
* @author Spencer Banasik
* @details This file...
* new line of this file...
*/


// Spencer Banasik
// Created: 12/14/2024
// Last Modified: 12/15/2024
// Description:
// Structure for engine
#ifndef GF_ENGINE_HPP
#define GF_ENGINE_HPP

#include <stdint.h>
#include <string>
#include <array>

#include <vulkan/vulkan.h>

#include <gl_manager.hpp>
#include <gf_vulkan.hpp>
#include <engine_types.hpp>

namespace gf {

/**
* @brief The main engine class.
* @author Spencer Banasik
* @details This class...
* extend the class description...
* @pre Vulkan can successfully initialize.
* @post Every resource will be freed.
*/
class Engine {
public:
    Engine();
    ~Engine();
    // Main Loop
    void run();

    Engine& get();
    Frame& get_current_frame() { return vk_context.frame_data.active_frames[frame_number % FRAME_OVERLAP]; }
    uint64_t get_frame() { return frame_number; }
    gl::WInputContext* get_glfw_context() { return &gl_context; }
    VkManager* get_vk_context() { return &vk_context; };

    bool should_render = true;
    // Do not declare these below contexts!
    VkExtent2D window_dims = { 1700, 900 };
    std::string title = "GarFish Engine";
    // ------------------
private:

    // This declaration order cannot be changed
    gl::GLManager gl_manager;
    gl::WInputContext gl_context;
    VkManager vk_context;
    // ------------------

    uint64_t frame_number = 0;
    bool should_kill_game = false;

    // Draw loop
    void draw();
    void draw_imgui(VkCommandBuffer cmd, VkImageView image_view);

    static Engine* loaded_engine;
    Engine(const Engine& other) = delete;
    Engine& operator=(const Engine& other) = delete;
};
}
#endif