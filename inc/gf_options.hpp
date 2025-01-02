// Spencer Banasik
// Created: 1/1/2025
// Last Modified: 1/1/2025
// Description:
// An aggregate structure for options that could be
// loaded in from a file
#ifndef GF_OPTIONS_HPP
#define GF_OPTIONS_HPP

#include <string>
#include <vulkan/vulkan.h>

namespace gf {
namespace option {
// Configurable options within the engine. Not exposed to consumer.
struct EngineOptions {
    VkExtent2D minimum_window_dims;
    VkExtent2D window_dims;
    VkExtent2D pixel_dims; // Dimensions of what is being rendered in pixels (not necessarily screen res)
    std::string window_name;

};
// Options that the consumer should be able to change
struct UserOptions {
    enum class WindowType {
        WINDOWED,
        FULLSCREEN,
        BORDERLESS
    } window_type;
    VkExtent2D desired_window_dims;
    float mouse_sensitivity;
    bool use_raw_mouse; // Only when mouse is disabled
};

struct Options {
    EngineOptions engine;
    UserOptions user;
};


void load_options_file(const char* file_path, EngineOptions& engine_options, UserOptions& user_options);
void generate_options_file(const char* file_path, EngineOptions& engine_options, UserOptions& user_options);
void generate_options_file_defaults(const char* file_path, EngineOptions& engine_options, UserOptions& user_options);


}
}
#endif