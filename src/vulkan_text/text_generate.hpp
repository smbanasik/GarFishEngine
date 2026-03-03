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

/**
 * @struct MeshBufferData
 * @brief A bundle that contains the vertices and indices needed as input for the upload_mesh function.
 */
struct MeshBufferData {
    std::vector<vkl::Vertex> vertices;
    std::vector<uint32_t> indices;
};

/**
 * @brief Generates squares of text to be used
 * @author Spencer Banasik
 * @param [in] text The string of text to be generated
 * @param [in] font_info Information obtained from the FontManager
 * @returns Populated vertex and index buffers courtesy of a MeshBufferData struct
 */
MeshBufferData generate_text_squares(const std::string& text, const vkh_font_manager::Font& font_info);

/**
 * @brief Generates squares of text to be used
 * @author Spencer 
 * @param [in] text_length
 * @returns Populated index buffer
 */
std::vector<uint32_t> generate_text_indices(const size_t text_length);

/**
 * @brief Generates squares of text to be used
 * @author Spencer Banasik
 * @details Fill in vertices and their positions.
 * @param [in] text The string of text to be generated
 * @param [in] font_info Information obtained from the FontManager
 * @returns Populated vertex and index buffers courtesy of a MeshBufferData struct
 */
std::vector<vkl::Vertex> generate_text_vertices(const std::string& text,
                                                const vkh_font_manager::Font& font_info);

/**
 * @brief Populates a quad's normals
 * @author Spencer Banasik
 * @param [in, out] quad 4 vertices whose normals are to be mutated
 */
inline void populate_text_quad_normal(std::span<vkl::Vertex, 4> quad) {
    quad[0].normal = {0, 0, 1};
    quad[1].normal = {0, 0, 1};
    quad[2].normal = {0, 0, 1};
    quad[3].normal = {0, 0, 1};
};

/**
 * @brief Populates a quad's color value
 * @author Spencer Banasik
 * @param [in, out] quad 4 vertices whose colors are to be mutated
 */
inline void populate_text_quad_color(std::span<vkl::Vertex, 4> quad) {
    quad[0].color = {1, 1, 1, 1};
    quad[1].color = {1, 1, 1, 1};
    quad[2].color = {1, 1, 1, 1};
    quad[3].color = {1, 1, 1, 1};
};

/**
 * @brief Generates squares of text to be used
 * @author Spencer Banasik
 * @details Uses information from the character to grab the corners of the character texture,
 * which are assigned to the quad's uv values.
 * @param [in, out] quad The quad to be mutated
 * @param [in] character The character which is used to get the texture
 * @param [in] texture The texture image from the font
 */
void populate_text_quad_texture(std::span<vkl::Vertex, 4> quad,
                                const vkh_font_data::Character& character,
                                const vkh_font_image::FontImage& texture);

}
#endif