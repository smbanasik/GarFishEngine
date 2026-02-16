/**
 * @file
 * @brief A series of functions to generate text squares
 * @author Spencer Banasik
 */
#ifndef VK_TEXT_GENERATE_HPP
#define VK_TEXT_GENERATE_HPP
#include <vector>
#include <string>
#include <stdint.h>
#include <span>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include<vulkan_low/types.hpp>
#include<vulkan_text/font_manager.hpp>

namespace vk_text {

struct MeshBufferData {
    std::vector<vkl::Vertex> vertices;
    std::vector<uint32_t> indices;
};

MeshBufferData generate_text_squares(const std::string& text, const vkh_font_manager::Font& font_info);
std::vector<uint32_t> generate_text_indices(const size_t text_length);
std::vector<vkl::Vertex> generate_text_vertices(const std::string& text,
                                                const vkh_font_manager::Font& font_info);

inline void populate_text_quad_normal(std::span<vkl::Vertex, 4> quad) {
    quad[0].normal = {0, 0, 1};
    quad[1].normal = {0, 0, 1};
    quad[2].normal = {0, 0, 1};
    quad[3].normal = {0, 0, 1};
};
inline void populate_text_quad_color(std::span<vkl::Vertex, 4> quad) {
    quad[0].color = {1, 1, 1, 1};
    quad[1].color = {1, 1, 1, 1};
    quad[2].color = {1, 1, 1, 1};
    quad[3].color = {1, 1, 1, 1};
};
void populate_text_quad_texture(std::span<vkl::Vertex, 4> quad,
                                const vkh_font_data::Character& character,
                                const vkh_font_image::FontImage& texture);

}
#endif