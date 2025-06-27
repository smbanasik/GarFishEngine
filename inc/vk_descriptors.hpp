/**
* @file
* @brief File that contains mouse WInput module.
* @author Spencer Banasik
* @todo This file has some structs that could use RAII.
* @todo Move descriptor discussion here to other places
*/
#ifndef VK_DESCRIPTORS_HPP
#define VK_DESCRIPTORS_HPP
#include <vector>
#include <stdint.h>
#include <span>
#include <deque>

#include <vulkan/vulkan.h>

// TODO: revamp this once we get into pipelines and full on shader usage

namespace gf {
namespace vk_desc {
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

struct DescriptorAllocatorGrowable {
    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    void init(VkDevice device, uint32_t initial_sets, std::span<PoolSizeRatio> pool_ratios);
    void clear_pools(VkDevice device);
    void destroy_pools(VkDevice device);

    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
private:
    VkDescriptorPool get_pool(VkDevice device);
    VkDescriptorPool create_pool(VkDevice device, uint32_t set_count, std::span<PoolSizeRatio> pool_ratio);

    std::vector<PoolSizeRatio> ratios;
    std::vector<VkDescriptorPool> full_pools;
    std::vector<VkDescriptorPool> available_pools;
    uint32_t sets_per_pool;
};

struct DescriptorWriter {
    std::deque<VkDescriptorImageInfo> image_infos;
    std::deque<VkDescriptorBufferInfo> buffer_infos;
    std::vector<VkWriteDescriptorSet> writes;

    void write_image(int binding, VkImageView view, VkSampler sampler, VkImageLayout layout, VkDescriptorType type);
    void write_buffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type);
    void clear();
    void update_set(VkDevice device, VkDescriptorSet set);
};
}
}
#endif
