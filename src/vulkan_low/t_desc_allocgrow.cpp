#include <t_desc_allocgrow.hpp>

void vkl_desc::DescriptorAllocatorGrowable::init(VkDevice device, uint32_t initial_sets, std::span<PoolSizeRatio> pool_ratios) {
    ratios.clear();
    for (auto r : pool_ratios)
        ratios.push_back(r);

    VkDescriptorPool new_pool = create_pool(device, initial_sets, pool_ratios);
    sets_per_pool = initial_sets * 1.5;
    available_pools.push_back(new_pool);
}
void vkl_desc::DescriptorAllocatorGrowable::clear_pools(VkDevice device) {
    for (auto p : available_pools)
        vkResetDescriptorPool(device, p, 0);
    for (auto p : full_pools) {
        vkResetDescriptorPool(device, p, 0);
        available_pools.push_back(p);
    }
    full_pools.clear();
}
void vkl_desc::DescriptorAllocatorGrowable::destroy_pools(VkDevice device) {
    for (auto p : available_pools)
        vkDestroyDescriptorPool(device, p, nullptr);
    for (auto p : full_pools) {
        vkDestroyDescriptorPool(device, p, nullptr);
    }
    available_pools.clear();
    full_pools.clear();
}

VkDescriptorSet vkl_desc::DescriptorAllocatorGrowable::allocate(VkDevice device, VkDescriptorSetLayout layout) {
    VkDescriptorPool used_pool = get_pool(device);

    VkDescriptorSetAllocateInfo alloc_info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    alloc_info.pNext = nullptr;
    alloc_info.descriptorPool = used_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;

    VkDescriptorSet ds;
    VkResult result = vkAllocateDescriptorSets(device, &alloc_info, &ds);

    if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {
        full_pools.push_back(used_pool);

        used_pool = get_pool(device);
        alloc_info.descriptorPool = used_pool;

        vkAllocateDescriptorSets(device, &alloc_info, &ds);
    }

    available_pools.push_back(used_pool);
    return ds;
}
VkDescriptorPool vkl_desc::DescriptorAllocatorGrowable::get_pool(VkDevice device) {
    VkDescriptorPool new_pool;
    if (available_pools.size() != 0) {
        new_pool = available_pools.back();
        available_pools.pop_back();
    }
    else {
        new_pool = create_pool(device, sets_per_pool, ratios);
        sets_per_pool = sets_per_pool * 1.5;
        if (sets_per_pool > 4092)
            sets_per_pool = 4092;
    }
    return new_pool;
}
VkDescriptorPool vkl_desc::DescriptorAllocatorGrowable::create_pool(VkDevice device, uint32_t set_count, std::span<PoolSizeRatio> pool_ratio) {
    std::vector<VkDescriptorPoolSize> pool_sizes;
    for (PoolSizeRatio ratio : ratios) {
        pool_sizes.push_back(VkDescriptorPoolSize{
            .type = ratio.type,
            .descriptorCount = uint32_t(ratio.ratio * set_count)
            });
    }

    VkDescriptorPoolCreateInfo pool_info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    pool_info.flags = 0;
    pool_info.maxSets = set_count;
    pool_info.poolSizeCount = (uint32_t)pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();

    VkDescriptorPool new_pool;
    vkCreateDescriptorPool(device, &pool_info, nullptr, &new_pool);
    return new_pool;
}