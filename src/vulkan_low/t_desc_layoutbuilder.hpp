/**
 * @file
 * @relates DescriptorLayoutBuilder
 */
#ifndef T_DESC_LAYOUTBUILDER_HPP
#define T_DESC_LAYOUTBUILDER_HPP

#include <vector>

#include <vulkan/vulkan.h>

namespace vkl_desc {
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
}
#endif

