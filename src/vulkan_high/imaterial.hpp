/**
* @file
* @brief Contains interface for building materials
* @author Spencer Banasik
* @related IBaseMaterial
*/
#ifndef VKH_IMATERIAL_HPP
#define VKH_IMATERIAL_HPP
#include <stdint.h>

#include <vulkan_low/mat_types.hpp>
#include <vulkan_low/images.hpp>
#include <vulkan_low/desc_allocgrow.hpp>

namespace gf {
class VkManager;
}

namespace vkh_mat {
/**
 * @class IMaterialResources
 * @brief Interface for a material's resources
 * @details A virtual destructor is needed as the bare minimum for
 * inheritance polymorphism to work. A child class should implement
 * a derivation of IMaterialResources, which can then be passed to 
 * the material's write_material function.
 */
struct IMaterialResources {
    virtual ~IMaterialResources() {};
};
/**
 * @class IBaseMaterial
 * @brief Base interface for all engine materials to have
 * @author Spencer Banasik
 * @details Every material MUST implement these functions.
 * The primary function is write_material, which produces a MaterialInstance.
 * The material should, at minimum, contain the pipelines and descriptor layout for the
 * creation of the MaterialInstace.
 * @todo This class should have a protected parameterized constructor to allow for
 * RAII.
 * @relates MaterialInstance
 */
struct IBaseMaterial {
    virtual ~IBaseMaterial() {};

    /**
     * @brief Initializes the material.
     * @param engine A pointer to VkManager to pass essential data.
     * @details Your material should have at minimum one pipeline, but optionall more for 
     * each type of MaterialPass.
     * 
     * The material will need to load its shaders through this function, often done with a hard coded path,
     * and build the descriptor layout here. These will go into the pipeline layout as apart of its descriptor
     * layouts. Once those are made, the pipelines can be built and the shaders may be destroyed.
     * 
     * @note On VkPipeline, these describe what configurations and shaders to run for a given object. These are fairly
     * expensive to use so limiting the amount of pipelines we create is good!
     * 
     * @note On VkShaderModule, these are processed shader files which are pre-compiled, which provide the GPU with
     * our shader programs.
     */
    virtual void build_pipelines(gf::VkManager* engine) = 0;

    /**
     * @brief Deletes the built pipelines and layouts.
     * @param device The driver to delete the resources from.
     */
    virtual void clear_resources(VkDevice device) = 0;

    /**
     * @brief Allows the device to be set without a constructor.
     * @param device The device to be passed.
     */
    virtual void set_device(VkDevice* device) = 0;

    /**
     * @brief Produce a material instance from the given parameters
     * @author Spencer Banasik
     * @param device The device that the descriptors are allocated on.
     * @param pass The way light passes through the material.
     * @param resources A set of resources unique to a given material.
     * @param descriptor_allocator Allocator to produce the instance's descriptor set.
     * @returns A material instance to be used with a mesh.
     * @todo the device may not be needed since we have set_device (for the RAII destructor)
     * 
     * @details The write material function needs to create a MaterialInstance. This is done by
     * assigning stored pipelines to the pipeline member variable, creating a descriptor set with
     * the allocator, and then updating that set with a DescriptorWriter and the IMaterialResources.
     */
    virtual vkl::MaterialInstance write_material(VkDevice device, vkl::MaterialPass pass,
        const IMaterialResources& resources,
        vkl_desc::DescriptorAllocatorGrowable& descriptor_allocator) = 0;
};

}
#endif
