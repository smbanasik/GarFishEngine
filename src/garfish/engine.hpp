/**
* @file
* @brief Main engine header file.
* @author Spencer Banasik
* @details This file holds the main engine class.
*/
#ifndef GF_ENGINE_HPP
#define GF_ENGINE_HPP

#include <stdint.h>
#include <string>
#include <array>

#include <vulkan/vulkan.h>

#include <window_input/wi_manager.hpp>
#include <garfish/vulkan.hpp>

namespace gf {

/**
* @class Engine
* @brief The main engine class.
* @author Spencer Banasik
*/
class Engine {
public:
    Engine();
    ~Engine();
    
    /**
     * @brief Main loop of engine.
     */
    void run();

    /**
     * @brief Get the singleton.
     * @returns A reference to the singleton.
     */
    static Engine& get();

    /**
     * @brief Return the current frame in use.
     * @returns The current frame.
     */
    vkl_frames::Frame& get_current_frame() { return vk_context.frame_data.active_frames[frame_number % FRAME_OVERLAP]; }

    /**
     * @brief Get frame number.
     * @returns The frame number. 
     */
    uint64_t get_frame() { return frame_number; }

    /**
     * @brief Get the glfw context with this engine.
     * @returns The glfw context.
     */
    wi::WInputContext* get_glfw_context() { return &wi_context; }

    /**
     * @brief Get the vulkan context with this engine.
     * @returns The vk context.
     */
    VkManager* get_vk_context() { return &vk_context; };

    bool should_render = true;
    // Do not declare these below contexts!
    VkExtent2D window_dims = { 1700, 900 };
    std::string title = "GarFish Engine";
    // ------------------
private:

    // This declaration order cannot be changed
    wi::WIManager wi_manager;
    wi::WInputContext wi_context;
    VkManager vk_context;
    // ------------------

    uint64_t frame_number = 0;
    bool should_kill_game = false;

    /**
     * @brief An additional draw function.
     * @todo Provide more details.
     */
    void draw();

    /**
     * @brief A draw function for imgui
     * @param [in, out] cmd The command buffer to recieve draw commands.
     * @param [in] image_view
     * @todo Provide more details
     */
    void draw_imgui(VkCommandBuffer cmd, VkImageView image_view);

    static Engine* loaded_engine;
    Engine(const Engine& other) = delete;
    Engine& operator=(const Engine& other) = delete;
};
}
#endif