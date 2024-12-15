// Spencer Banasik
// Created: 12/14/2024
// Last Modified: 12/14/2024
// Description:
// Structure for engine
#ifndef GF_ENGINE_HPP
#define GF_ENGINE_HPP

#include <stdint.h>
#include <string>

#include <vulkan/vulkan.h>

// Forward Declares
class GLFWwindow;

namespace GF {
class Engine {
public:
    Engine();
    ~Engine();
    // Main Loop
    void run();

    Engine& get();

private:

    GLFWwindow* window = nullptr;
    uint64_t frame_number = 0;
    VkExtent2D window_dims = {1700, 900};
    std::string title = "GarFish Engine";
    bool is_initialized = false;
    bool should_render = true;
    bool should_kill_game = false;

    // Handle window initialization
    GLFWwindow* init_window(GLFWwindow* window);
    // Draw loop
    void draw();

    static Engine* loaded_engine;
    Engine(const Engine& other) {};
    Engine& operator=(const Engine& other) {};
};
}
#endif