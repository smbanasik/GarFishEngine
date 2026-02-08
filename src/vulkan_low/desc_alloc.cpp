#include <vulkan_low/desc_alloc.hpp>

void vkl_desc::DescriptorAllocator::init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> pool_ratios) {

    std::vector<VkDescriptorPoolSize> pool_sizes;
    for (PoolSizeRatio ratio : pool_ratios) {
        pool_sizes.push_back(VkDescriptorPoolSize{
            .type = ratio.type,
            .descriptorCount = uint32_t(ratio.ratio * maxSets)
            });
    }

    VkDescriptorPoolCreateInfo pool_info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    pool_info.flags = 0;
    pool_info.maxSets = maxSets;
    pool_info.poolSizeCount = (uint32_t)pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();

    vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
}
void vkl_desc::DescriptorAllocator::clear_descriptors(VkDevice device) {
    vkResetDescriptorPool(device, pool, 0);
}
void vkl_desc::DescriptorAllocator::destroy_pool(VkDevice device) {
    vkDestroyDescriptorPool(device, pool, nullptr);
}

VkDescriptorSet vkl_desc::DescriptorAllocator::allocate(VkDevice device, VkDescriptorSetLayout layout) {
    VkDescriptorSetAllocateInfo alloc_info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    alloc_info.pNext = nullptr;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;

    VkDescriptorSet ds;
    vkAllocateDescriptorSets(device, &alloc_info, &ds);

    return ds;
}