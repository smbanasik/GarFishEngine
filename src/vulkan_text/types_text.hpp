#ifndef VK_TEXT_TYPES_TEXT_HPP
#define VK_TEXT_TYPES_TEXT_HPP

#include <string>

#include <vulkan_low/types.hpp>

namespace vk_text {

/**
 * @struct TextMaterial
 * @brief Abstraction of MaterialInstance
 */
struct TextMaterial {
    vkl::MaterialInstance data;
};

/**
 * @struct Word
 * @brief A surface for text.
 * @details Contains the data for rendering a set of characters that all use the same TextMaterial, which
 * will generally have the same size, font, and color.
 */
struct Word {
    uint32_t start_idx;
    uint32_t count;
    std::shared_ptr<TextMaterial> material;
    vkl::Bounds bounds; // TODO: determine if we need this right now
};

/**
 * @struct TextAsset
 * @brief A text asset.
 * @details Contains words, the mesh data itself,
 * and the name of the text mesh. Note that the surfaces just differentiate data in
 * the mesh_buffers and provide materials, the actual vertex and index data is
 * in the mesh_buffers.
 */
struct TextAsset {
    std::string name;
    std::vector<Word> surfaces;
    vkl::GPUMeshBuffers mesh_buffers;
};

}
#endif