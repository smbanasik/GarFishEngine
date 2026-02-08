/**
* @file
* @brief Holds render related bundles.
* @author Spencer Banasik
*/
#ifndef VK_RENDERABLE_HPP
#define VK_RENDERABLE_HPP

#include <vulkan_low/types.hpp>

namespace vkh_render {

/**
 * @struct GLTFMaterial
 * @brief Abstraction of MaterialInstance
 */
struct GLTFMaterial {
    vkl::MaterialInstance data;
};

/**
 * @struct GeoSurface
 * @brief A surface on a mesh.
 * @details Acts as a submesh for a given mesh asset, containing its start index
 * in the buffer, its count, boundaries, and the material to be used with the surface.
 */
struct GeoSurface {
    uint32_t start_idx;
    uint32_t count;
    vkl::Bounds bounds; // TODO: need to create a version of surfaces without this
    std::shared_ptr<GLTFMaterial> material;
};

/**
 * @struct MeshAsset
 * @brief A mesh asset.
 * @details Contains submesh surfaces, the mesh data itself,
 * and the name of the mesh. Note that the surfaces just differentiate data in
 * the mesh_buffers and provide materials, the actual vertex and index data is
 * in the mesh_buffers.
 */
struct MeshAsset {
    std::string name;
    std::vector<GeoSurface> surfaces;   
    vkl::GPUMeshBuffers mesh_buffers;
};

}
#endif