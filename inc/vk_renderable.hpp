/**
* @file
* @brief Contains higher level abstractions for rendering.
* @author Spencer Banasik
*/
#ifndef VK_RENDERABLE_HPP
#define VK_RENDERABLE_HPP

#include <memory>
#include <vector>
#include <array>
#include <string>

#include <glm/mat4x4.hpp>

#include <vk_types.hpp>

namespace gf {
namespace vk_render {

/**
 * @struct GLTFMaterial
 * @brief Abstraction of MaterialInstance
 */
struct GLTFMaterial {
    MaterialInstance data;
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
    Bounds bounds; // TODO: need to create a version of surfaces without this
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
    GPUMeshBuffers mesh_buffers;
};

/**
 * @struct DrawContext
 * @brief Bundle for different surface types.
 */
struct DrawContext {
    std::vector<RenderObject> opaque_surfaces;
    std::vector<RenderObject> transparent_surfaces;
    std::vector<RenderObject> static_surfaces; // Meant for UI elements, shouldn't affected by render proj matrix. No culling.
};

/**
 * @struct Quad
 * @brief A rectangle of vertices with their respective indexes.
 * @author Spencer Banasik
 */
struct Quad {
public:
    std::array<Vertex, 4> verts;
    std::array<uint32_t, 6> indices = { 0, 1, 2, 2, 3, 0 };

    /**
     * @brief Creates a textured rectangle
     * @param idx_offset [in] Offset for indexes
     * @param vert_rect  [in] Position of rectangle
     * @param texture_rect [in] Texture coordinates
     * @return A Quad fitted with the parameters
     * @todo details
     */
    static Quad generate_textured_quad(uint32_t idx_offset, glm::vec4 vert_rect, glm::vec4 texture_rect);
};

/**
 * @struct IRenderable
 * @brief Interface for any renderable objects.
 */
struct IRenderable {
    virtual void draw(const glm::mat4& top_matrix, DrawContext& ctx) = 0;
};

/**
 * @struct Node
 * @brief A node in the scene tree
 */
struct Node : public IRenderable {
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;

    glm::mat4 local_transform;
    glm::mat4 world_transform;

    void refresh_transform(const glm::mat4& parent_matrix);
    virtual void draw(const glm::mat4& top_matrix, DrawContext& ctx);
};
/**
 * @struct MeshNode
 * @brief A specialized node which has a MeshAsset.
 */
struct MeshNode : public Node {
    std::shared_ptr<MeshAsset> mesh;
    virtual void draw(const glm::mat4& top_matrix, DrawContext& ctx);
};

}
}
#endif
