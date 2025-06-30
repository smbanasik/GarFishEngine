#include <t_desc_layoutbuilder.hpp>

void vkl_desc::DescriptorLayoutBuilder::add_binding(uint32_t location, VkDescriptorType type) {
    VkDescriptorSetLayoutBinding new_bind{};
    new_bind.binding = location;
    new_bind.descriptorCount = 1;
    new_bind.descriptorType = type;

    bindings.push_back(new_bind);
}
void vkl_desc::DescriptorLayoutBuilder::clear() {
    bindings.clear();
}
VkDescriptorSetLayout vkl_desc::DescriptorLayoutBuilder::build(VkDevice device, VkShaderStageFlags shader_stages, void* pNext, VkDescriptorSetLayoutCreateFlags flags) {
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