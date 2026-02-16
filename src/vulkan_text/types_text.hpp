#ifndef VK_TEXT_TYPES_TEXT_HPP
#define VK_TEXT_TYPES_TEXT_HPP

#include <string>

#include <vulkan_low/types.hpp>

namespace vk_text {

struct TextMaterial {
    vkl::MaterialInstance data;
};

struct Word {
    uint32_t start_idx;
    uint32_t count;
    std::shared_ptr<TextMaterial> material;
    vkl::Bounds bounds; // TODO: determine if we need this right now
};

struct TextAsset {
    std::string name;
    std::vector<Word> surfaces;
    vkl::GPUMeshBuffers mesh_buffers;
};

}
#endif