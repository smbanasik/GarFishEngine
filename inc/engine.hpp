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

#include <gf_window.hpp>
#include <gf_vulkan.hpp>
#include <engine_types.hpp>

// Forward Declares
struct GLFWwindow;

namespace GF {

class Engine {
public:
    Engine();
    ~Engine();
    // Main Loop
    void run();

    Engine& get();
    FrameData& get_current_frame() { return vk_context.active_frames[frame_number % FRAME_OVERLAP]; }
    uint64_t get_frame() { return frame_number; }

private:

    // This declaration order cannot be changed
    VkExtent2D window_dims = { 1700, 900 };
    std::string title = "GarFish Engine";
    GLFWManager gl_context;
    VkManager vk_context;
    // ------------------

    uint64_t frame_number = 0;
    bool should_render = true;
    bool should_kill_game = false;

    // Draw loop
    void draw();

    static Engine* loaded_engine;
    Engine(const Engine& other) = delete;
    Engine& operator=(const Engine& other) = delete;
};
}
#endif