#include <vulkan_high/default_data.hpp>

#include <array>
#include <stdint.h>

#include <glm/glm.hpp>

gs::DefaultData::DefaultData(VkDevice device, vkh::ImageManager& image_alloc) : device(device) {

    uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
    uint32_t gray = glm::packUnorm4x8(glm::vec4(0.66f, 0.66f, 0.66f, 1));
    uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
    uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
    std::array<uint32_t, 16 * 16> pixels;
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
        }
    }

    image_alloc.add_texture_from_data("white", (void*)&white, VkExtent3D{1, 1, 1});
    image_alloc.add_texture_from_data("gray", (void*)&gray, VkExtent3D{1, 1, 1});
    image_alloc.add_texture_from_data("black", (void*)&black, VkExtent3D{1, 1, 1});
    image_alloc.add_texture_from_data("magenta", (void*)&magenta, VkExtent3D{1, 1, 1});
    image_alloc.add_texture_from_data("error_checkerboard", pixels.data(), VkExtent3D{1, 1, 1});

    VkSamplerCreateInfo sampl{.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    sampl.magFilter = VK_FILTER_NEAREST;
    sampl.minFilter = VK_FILTER_NEAREST;
    vkCreateSampler(device, &sampl, nullptr, &default_sampler_nearest);
    sampl.magFilter = VK_FILTER_LINEAR;
    sampl.minFilter = VK_FILTER_LINEAR;
    vkCreateSampler(device, &sampl, nullptr, &default_sampler_linear);
}

gs::DefaultData::~DefaultData() {

    vkDestroySampler(device, default_sampler_nearest, nullptr);
    vkDestroySampler(device, default_sampler_linear, nullptr);
}