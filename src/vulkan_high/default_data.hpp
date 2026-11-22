#ifndef GS_DEFAULT_DATA_HPP
#define GS_DEFAULT_DATA_HPP

#include <vulkan/vulkan.h>

#include <vulkan_high/image_manager.hpp>
#include <vulkan_low/mat_types.hpp>

namespace gs {
class DefaultData {
  public:
    DefaultData(VkDevice device, vkh::ImageManager& image_alloc);
    ~DefaultData();

    DefaultData(const DefaultData& other) = delete;
    DefaultData operator=(const DefaultData& other) = delete;

    VkSampler default_sampler_linear;
    VkSampler default_sampler_nearest;

  private:
    VkDevice device;
};

} // namespace gs

#endif