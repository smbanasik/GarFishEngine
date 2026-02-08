/**
 * @file
 * @relates DescriptorWriter
 */
#ifndef T_DESC_WRITER_HPP
#define T_DESC_WRITER_HPP

#include <deque>
#include <vector>

#include <vulkan/vulkan.h>

namespace vkl_desc {
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
#endif

