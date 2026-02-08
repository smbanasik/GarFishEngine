/**
 * @file
 * @relates DescriptorWriter
 *@todo Move descriptor discussion here to other places
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
#ifndef T_DESC_ALLOC_HPP
#define T_DESC_ALLOC_HPP

#include <span>
#include <vector>

#include <vulkan/vulkan.h>

namespace vkl_desc {
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
}
#endif

