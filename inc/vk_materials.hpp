// Spencer Banasik
// Created: 12/31/2024
// Last Modified: 12/31/2024
// Description:
// Higher level data types for materials
// The associated cpp is also a more realworld usecase for 
// the tools we have been developing
#ifndef VK_MATERIALS_HPP
#define VK_MATERIALS_HPP
#include <stdint.h>

#include <vk_types.hpp>
#include <vk_images.hpp>

namespace gf{
class VkManager;

namespace vk_mat {
struct IMaterialBase {
    MaterialPipeline opaque_pipeline;
    MaterialPipeline transparent_pipeline;

    VkDescriptorSetLayout material_layout;

    struct MaterialConstants;
    struct MaterialResources;

    vk_desc::DescriptorWriter writer;

    virtual void build_pipelines(VkManager* engine) = 0;
    virtual void clear_resources(VkDevice device) = 0;

    virtual MaterialInstance write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, vk_desc::DescriptorAllocatorGrowable& descriptor_allocator) = 0;
};

struct GLTFMetallic_Roughness {
    MaterialPipeline opaque_pipeline;
    MaterialPipeline transparent_pipeline;

    VkDescriptorSetLayout material_layout;

    struct MaterialConstants {
        glm::vec4 color_factors;
        glm::vec4 metal_rough_factors;
        //padding, we need it anyway for uniform buffers
        glm::vec4 extra[14];
    };

    struct MaterialResources {
        vk_img::AllocatedImage color_image;
        VkSampler color_sampler;
        vk_img::AllocatedImage metal_rough_image;
        VkSampler metal_rough_sampler;
        VkBuffer data_buffer;
        uint32_t data_buffer_offset;

        MaterialResources(vk_img::ImageBufferAllocator& allocator)
            : color_image(allocator), metal_rough_image(allocator) {};
    };

    vk_desc::DescriptorWriter writer;

    void build_pipelines(VkManager* engine);
    void clear_resources(VkDevice device);

    MaterialInstance write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, vk_desc::DescriptorAllocatorGrowable& descriptor_allocator);
};
struct MaterialImage {
    MaterialPipeline opaque_pipeline;
    MaterialPipeline transparent_pipeline;

    VkDescriptorSetLayout material_layout;

    struct MaterialResources {
        vk_img::AllocatedImage color_image;
        VkSampler color_sampler;

        MaterialResources(vk_img::ImageBufferAllocator& allocator)
            : color_image(allocator) {};
    };

    vk_desc::DescriptorWriter writer;

    void build_pipelines(VkManager* engine);
    void clear_resources(VkDevice device);

    MaterialInstance write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, vk_desc::DescriptorAllocatorGrowable& descriptor_allocator);
};
/*
struct MaterialImageTriColor {

};
*/

}
}
#endif
