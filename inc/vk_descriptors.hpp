// Spencer Banasik
// Created: 12/27/2024
// Last Modified: 12/27/2024
// Description:
// Data types for handling descriptors
#ifndef VK_DESCRIPTORS_HPP
#define VK_DESCRIPTORS_HPP
#include <vector>
#include <stdint.h>
#include <span>

#include <vulkan/vulkan.h>

// TODO: revamp this once we get into pipelines and full on shader usage

namespace gf {

// Builder for descriptor layouts, which are a pack of bindings for a shader
struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    void add_binding(uint32_t location, VkDescriptorType type);
    void clear();
    VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shader_stages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
};

// Abstraction of VkDescriptorPool so we can have multiple that are specialized for what we want to do
// TODO: RAII this
// TODO: also review function further
struct DescriptorAllocator {

    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;

    void init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> pool_ratios);
    void clear_descriptors(VkDevice device);
    void destroy_pool(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};
}
#endif
