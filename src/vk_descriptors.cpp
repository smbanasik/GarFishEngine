// Spencer Banasik
// Created: 12/27/2024
// Last Modified: 12/30/2024
#include <vk_descriptors.hpp>

void gf::vk_desc::DescriptorLayoutBuilder::add_binding(uint32_t location, VkDescriptorType type) {
    VkDescriptorSetLayoutBinding new_bind{};
    new_bind.binding = location;
    new_bind.descriptorCount = 1;
    new_bind.descriptorType = type;

    bindings.push_back(new_bind);
}
void gf::vk_desc::DescriptorLayoutBuilder::clear() {
    bindings.clear();
}
VkDescriptorSetLayout gf::vk_desc::DescriptorLayoutBuilder::build(VkDevice device, VkShaderStageFlags shader_stages, void* pNext, VkDescriptorSetLayoutCreateFlags flags) {
    for (auto& b : bindings) {
        b.stageFlags |= shader_stages;
    }

    VkDescriptorSetLayoutCreateInfo info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    info.pNext = pNext;

    info.pBindings = bindings.data();
    info.bindingCount = (uint32_t)bindings.size();
    info.flags = flags;

    VkDescriptorSetLayout set;
    vkCreateDescriptorSetLayout(device, &info, nullptr, &set);

    return set;
}

void gf::vk_desc::DescriptorAllocator::init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> pool_ratios) {
    
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
void gf::vk_desc::DescriptorAllocator::clear_descriptors(VkDevice device) {
    vkResetDescriptorPool(device, pool, 0);
}
void gf::vk_desc::DescriptorAllocator::destroy_pool(VkDevice device) {
    vkDestroyDescriptorPool(device, pool, nullptr);
}

VkDescriptorSet gf::vk_desc::DescriptorAllocator::allocate(VkDevice device, VkDescriptorSetLayout layout) {
    VkDescriptorSetAllocateInfo alloc_info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    alloc_info.pNext = nullptr;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout;

    VkDescriptorSet ds;
    vkAllocateDescriptorSets(device, &alloc_info, &ds);

    return ds;
}


void gf::vk_desc::DescriptorAllocatorGrowable::init(VkDevice device, uint32_t initial_sets, std::span<PoolSizeRatio> pool_ratios) {
    ratios.clear();
    for (auto r : pool_ratios)
        ratios.push_back(r);

    VkDescriptorPool new_pool = create_pool(device, initial_sets, pool_ratios);
    sets_per_pool = initial_sets * 1.5;
    available_pools.push_back(new_pool);
}
void gf::vk_desc::DescriptorAllocatorGrowable::clear_pools(VkDevice device) {
    for (auto p : available_pools)
        vkResetDescriptorPool(device, p, 0);
    for (auto p : full_pools) {
        vkResetDescriptorPool(device, p, 0);
        available_pools.push_back(p);
    }
    full_pools.clear();
}
void gf::vk_desc::DescriptorAllocatorGrowable::destroy_pools(VkDevice device) {
    for (auto p : available_pools)
        vkDestroyDescriptorPool(device, p, nullptr);
    for (auto p : full_pools) {
        vkDestroyDescriptorPool(device, p, nullptr);
    }
    available_pools.clear();
    full_pools.clear();
}

VkDescriptorSet gf::vk_desc::DescriptorAllocatorGrowable::allocate(VkDevice device, VkDescriptorSetLayout layout) {
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
VkDescriptorPool gf::vk_desc::DescriptorAllocatorGrowable::get_pool(VkDevice device) {
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
VkDescriptorPool gf::vk_desc::DescriptorAllocatorGrowable::create_pool(VkDevice device, uint32_t set_count, std::span<PoolSizeRatio> pool_ratio) {
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

void gf::vk_desc::DescriptorWriter::write_image(int binding, VkImageView view, VkSampler sampler, VkImageLayout layout, VkDescriptorType type) {
    VkDescriptorImageInfo& info = image_infos.emplace_back(VkDescriptorImageInfo{
        .sampler = sampler,
        .imageView = view,
        .imageLayout = layout
        });

    VkWriteDescriptorSet write = { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write.dstBinding = binding;
    write.dstSet = VK_NULL_HANDLE;
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pImageInfo = &info;

    writes.push_back(write);
}
void gf::vk_desc::DescriptorWriter::write_buffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type) {
    VkDescriptorBufferInfo& info = buffer_infos.emplace_back(VkDescriptorBufferInfo{
        .buffer = buffer,
        .offset = offset,
        .range = size
        });
    VkWriteDescriptorSet write = { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    write.dstBinding = binding;
    write.dstSet = VK_NULL_HANDLE;
    write.descriptorCount = 1;
    write.descriptorType = type;
    write.pBufferInfo = &info;

    writes.push_back(write);
}
void gf::vk_desc::DescriptorWriter::clear() {
    writes.clear();
    image_infos.clear();
    buffer_infos.clear();
}
void gf::vk_desc::DescriptorWriter::update_set(VkDevice device, VkDescriptorSet set) {

    for (VkWriteDescriptorSet& write : writes) {
        write.dstSet = set;
    }

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}