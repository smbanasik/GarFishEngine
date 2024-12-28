// Spencer Banasik
// Created: 12/28/2024
// Last Modified: 12/28/2024
#include <vk_pipelines.hpp>

#include <fstream>
#include <stdint.h>
#include <vector>

#include <vk_initializers.hpp>

bool gf::vk_pipe::load_shader_module(const char* file_path, VkDevice device, VkShaderModule* shader_module) {

    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        return false;

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);
    file.close();

    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.codeSize = buffer.size() * sizeof(uint32_t);
    create_info.pCode = buffer.data();

    VkShaderModule shader;
    if (vkCreateShaderModule(device, &create_info, nullptr, &shader) != VK_SUCCESS)
        return false;

    *shader_module = shader;
    return true;
}