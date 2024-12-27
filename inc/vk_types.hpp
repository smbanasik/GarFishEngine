// Spencer Banasik
// Created: 12/18/2024
// Last Modified: 12/18/2024
// Description:
// Data types meant for gf_vulkan
#ifndef VK_TYPES_HPP
#define VK_TYPES_HPP

#include <vector>
#include <stdint.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace gf {
// All swapchain information bundled together
struct SwapChain {
    VkSwapchainKHR swapchain{};
    VkFormat swapchain_format{};
    VkExtent2D swapchain_extent{};
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
};
// All data for a given frame bundled together
struct FrameData {
    VkCommandPool command_pool{};
    VkCommandBuffer command_buffer{};
    VkFence render_fence{};
    VkSemaphore swapchain_semaphore{}, render_semaphore{};
};
// All data for an image bundled together
struct AllocatedImage {
    VkImage image;
    VkImageView image_view;
    VmaAllocation allocation;
    VkExtent3D image_size;
    VkFormat image_format;
};
// Pipeline descriptor
struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    inline void add_binding(uint32_t binding, VkDescriptorType type) {
        VkDescriptorSetLayoutBinding new_bind{};
        new_bind.binding = binding;
        new_bind.descriptorCount = 1;
        new_bind.descriptorType = type;

        bindings.push_back(new_bind);
    }
    inline void clear() {
        bindings.clear();
    }
    inline VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shader_stages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0) {
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
};
// Descriptor allocator
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
