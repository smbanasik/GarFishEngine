#ifndef GS_DEFAULT_DATA_HPP
#define GS_DEFAULT_DATA_HPP

#include <vulkan_low/mat_types.hpp>

namespace gs {
class DefaultData {
  public:
    vkl::MaterialInstance default_data;
    VkSampler default_sampler_linear;
    VkSampler default_sampler_nearest;

    void init_default_data();
};

} // namespace gs

#endif