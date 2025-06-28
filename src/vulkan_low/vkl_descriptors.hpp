/**
* @file
* @brief File that contains mouse WInput module.
* @author Spencer Banasik
* @todo This file has some structs that could use RAII.
* @todo Move descriptor discussion here to other places
* @note ## On Descriptors
* Descriptors are how Vulkan provides data to shaders. You can note in a shader's 
* source code that they have set and binding layouts for input. On the C++ side, these are
* our descriptor sets. A descriptor is a handle to a resource such as a buffer or an image, along
* with some additional info such as it's size or type. 
* 
* A VkDescriptorSet is a group of these handles bound together for a shader. 
* VkDescriptorSets are allocated via a VkDescriptorPool (similar to command buffers and command pools),
* and are defined by a VkDescriptorLayout (think of the layout as a class, and the set as an object),
* which specifies what the set will hold. Once the VkDescriptorSet is defined, it is bound to a 
* pipeline using VkBindDescriptorSets.
* 
* The spec guarantees 4 sets, but each set slot has a cost, so less is better.
*/
#ifndef VKL_DESCRIPTORS_HPP
#define VKL_DESCRIPTORS_HPP
#include <vector>
#include <stdint.h>
#include <span>
#include <deque>

#include <vulkan/vulkan.h>

namespace gf {
namespace vk_desc {
/**
 * struct DescriptorLayoutBuilder
 * @brief Builter for a description layout.
 * @details Holds an array of bindings until it is built.
 * Once built it produces a descriptor set layout.
 */
struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    /**
     * @brief Add a binding to the layout builder.
     * @param location The binding number for the shader.
     * @param type The data type of the descriptor.
     */
    void add_binding(uint32_t location, VkDescriptorType type);

    /**
     * @brief Clear the bindings of the builder.
     */
    void clear();

    /**
     * @brief Construct a VkDescriptorSetLayout with the parameters and bindings.
     * @param device The device to create the descriptor set with.
     * @param shader_stages Which shaders the layout will be used in.
     * @param pNext Optional for addons
     * @param flags Additional configuration for the descriptor set.
     * @returns A usable VkDescriptorSetLayout
     */
    VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shader_stages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
};

/**
 * @struct DescriptorAllocator
 * @brief An abstraction of VkDescriptorPool to allocate descriptors.
 * @todo This should feature RAII.
 */
struct DescriptorAllocator {

    /**
     * @struct PoolSizeRatio
     * @brief Specifies a type and the ratio (out of 1.0) for that type.
     */
    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;

    /**
     * @brief Initalize a VkDescriptorPool with 
     * @param device Driver that the pool belongs to.
     * @param maxSets The maximum number of descriptor sets.
     * @param pool_ratios Ratios (that should add up to 1.0) for the types of descriptor sets.
     */
    void init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> pool_ratios);

    /**
     * @brief Clear the allocated descriptors sets.
     * @param device Driver that the descriptor sets belong to.
     */
    void clear_descriptors(VkDevice device);

    /**
     * @brief Destroys the pool.
     * @param device Driver that the pool belongs to.
     */
    void destroy_pool(VkDevice device);

    /**
     * @brief Given a layout and a device, produce a VkDescriptor Set.
     * @param device The driver that the pool belongs to.
     * @param layout The layout specifying the set.
     * @return A descriptor set ready to recieve handles according to the layout.
     */
    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};

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

/**
 * @struct DescriptorWriter
 * @brief An interface to simplify the process of writing to descriptors.
 * @todo Interface could use some work, this doesn't need to be all public.
 * @todo ensure we document every function
 * @details Use the write_image and write_buffer functions to add to the list of writes.
 * Once completed, use update_set to update the descriptor set.
 */
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
