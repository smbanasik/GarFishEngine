// Spencer Banasik
// Created: 12/28/2024
// Last Modified: 12/28/2024
// Description:
// Code for vulkan pipelines
#ifndef VK_PIPELINES_HPP
#define VK_PIPELINES_HPP

#include <vulkan/vulkan.h>

namespace gf {
namespace vk_pipe {

bool load_shader_module(const char* file_path, VkDevice device, VkShaderModule* shader_module);


}
}
#endif