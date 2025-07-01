/**
* @file
* @brief Types used in low level text implementation
* @author Spencer Banasik
*/
#ifndef VKH_TEXTLOW_TYPES_HPP
#define VKH_TEXTLOW_TYPES_HPP

#include <string>
#include <vector>
#include <stdint.h>
#include <memory>
#include <array>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vkl_types.hpp>
#include <vkl_mat_types.hpp>

namespace vkh_txtlow {

/**
 * @brief Contains the texture position and spacing data for a character
 */
struct Character {
    glm::vec2 texture_position;
    glm::ivec2 size;
    glm::ivec3 padding;
};

/**
 * @brief Holds the font's texture, character data, and the font's material instance
 */
struct Font {
    vkl_res::AllocatedImage texture;
    std::array<Character, 128> chars;
    vkl::MaterialInstance material;
};

/**
 * @brief Place holder in case we need more text config
 */
struct TextMat {
    Font* font;
};

/**
 * @brief A set of characters containing the same material.
 * @author Spencer Banasik
 * @details A word can be thought of as a surface, where a group of vertices 
 * all have the same material. Words do not necessarily have to be visually connected, 
 * they could be separated by other text, but they must be contiguous in memory.
 */
struct Word {
    uint32_t start_idx;
    uint32_t count;
    TextMat material;
};

/**
 * @brief A static passage ready to be converted into a render object.
 * @author Spencer Banasik
 */
struct TextAsset {
    std::string name;
    std::vector<Word> words;
    vkl::GPUMeshBuffers text_buffers;
};
}
#endif
