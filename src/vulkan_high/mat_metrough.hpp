/**
* @file
* @brief Contains interface for building materials
* @author Spencer Banasik
* @related GLTFMetallic_Roughness
*/
#ifndef MAT_METROUGH_HPP
#define MAT_METROUGH_HPP

#include <vkh_imaterial.hpp>

namespace vkh_mat {
struct GLTFMetallic_Roughness : public IBaseMaterial {
public:

    GLTFMetallic_Roughness();
    GLTFMetallic_Roughness(VkDevice* device);
    GLTFMetallic_Roughness(gf::VkManager* engine);
    GLTFMetallic_Roughness(GLTFMetallic_Roughness& other) = delete;
    GLTFMetallic_Roughness(GLTFMetallic_Roughness&& other) noexcept;
    GLTFMetallic_Roughness& operator=(GLTFMetallic_Roughness& other) = delete;
    GLTFMetallic_Roughness& operator=(GLTFMetallic_Roughness&& other) noexcept {
        if (this == &other)
            return *this;
        opaque_pipeline = std::move(other.opaque_pipeline);
        transparent_pipeline = std::move(other.transparent_pipeline);
        material_layout = std::move(other.material_layout);
        writer = std::move(other.writer);
        device = std::move(other.device);
        other.device = nullptr;
        return *this;
    }
    ~GLTFMetallic_Roughness();

    void set_device(VkDevice* device) override { this->device = device; };
    void build_pipelines(gf::VkManager* engine) override;
    gf::MaterialInstance write_material(VkDevice device, gf::MaterialPass pass, const IMaterialResources& resources,
        gf::vk_desc::DescriptorAllocatorGrowable& descriptor_allocator) override;

    struct MaterialConstants {
        glm::vec4 color_factors;
        glm::vec4 metal_rough_factors;
        //padding, we need it anyway for uniform buffers
        glm::vec4 extra[14];
    };

    struct MaterialResources : public IMaterialResources {
        vk_img::AllocatedImage color_image;
        VkSampler color_sampler;
        vk_img::AllocatedImage metal_rough_image;
        VkSampler metal_rough_sampler;
        VkBuffer data_buffer;
        uint32_t data_buffer_offset;

        MaterialResources(const vk_img::ImageBufferAllocator& allocator)
            : color_image(allocator), metal_rough_image(allocator) {
            color_sampler = nullptr;
            metal_rough_sampler = nullptr;
            data_buffer = nullptr;
            data_buffer_offset = 0;
        };
    };

    gf::MaterialPipeline opaque_pipeline;
    gf::MaterialPipeline transparent_pipeline;
    VkDescriptorSetLayout material_layout;
    gf::vk_desc::DescriptorWriter writer;

private:
    void clear_resources(VkDevice device) override;
    VkDevice* device;
};

}
#endif
