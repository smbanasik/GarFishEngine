/**
* @file
* @brief Contains interface for building materials
* @author Spencer Banasik
* @related MaterialImage
*/
#ifndef MAT_IMG_HPP
#define MAT_IMG_HPP

#include <vkh_imaterial.hpp>
#include <t_desc_writer.hpp>
#include <vkl_mat_types.hpp>

namespace vkh_mat {
struct MaterialImage : public IBaseMaterial {
public:

    MaterialImage();
    MaterialImage(VkDevice* device);
    MaterialImage(gf::VkManager* engine);
    MaterialImage(MaterialImage& other) = delete;
    MaterialImage(MaterialImage&& other) noexcept;
    MaterialImage& operator=(MaterialImage& other) = delete;
    MaterialImage& operator=(MaterialImage&& other) noexcept {
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
    ~MaterialImage();

    void set_device(VkDevice* device) override { this->device = device; };
    void build_pipelines(gf::VkManager* engine) override;
    vkl::MaterialInstance write_material(VkDevice device, vkl::MaterialPass pass, const IMaterialResources& resources,
        vkl_desc::DescriptorAllocatorGrowable& descriptor_allocator) override;

    struct MaterialResources : public IMaterialResources {
        vkl_res::AllocatedImage color_image;
        VkSampler color_sampler;

        MaterialResources(const vkl_res::ImageBufferAllocator& allocator)
            : color_image(allocator) {
            color_sampler = nullptr;
        };
    };

    vkl::MaterialPipeline opaque_pipeline;
    vkl::MaterialPipeline transparent_pipeline;
    VkDescriptorSetLayout material_layout;
    vkl_desc::DescriptorWriter writer;

private:
    void clear_resources(VkDevice device) override;
    VkDevice* device;
};
}
#endif
