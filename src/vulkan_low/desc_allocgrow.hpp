/**
 * @file
 * @relates DescriptorAllocatorGrowable
 */
#ifndef T_DESC_ALLOCGROW_HPP
#define T_DESC_ALLOCGROW_HPP

#include <span>
#include <vector>

#include <vulkan/vulkan.h>

namespace vkl_desc {
/**
 * @struct DescriptorAllocatorGrowable
 * @brief A growable allocator with multiple pools.
 * @todo This should feature RAII.
 */
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
}
#endif

