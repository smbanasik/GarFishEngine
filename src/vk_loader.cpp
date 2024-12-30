// Spencer Banasik
// Created: 12/29/2024
// Last Modified: 12/29/2024
#include <vk_loader.hpp>

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <stdint.h>

#include <stb_image.h>
#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>

#include <gf_vulkan.hpp>
#include <vk_initializers.hpp>
#include <vk_types.hpp>

std::optional<std::vector<std::shared_ptr<gf::vk_loader::MeshAsset>>> gf::vk_loader::load_gltf_meshes(VkManager* vk_context, std::filesystem::path filepath) {
    
    std::cout << "| INFO: loading GLTF: " << filepath << "\n";

    auto data = fastgltf::GltfDataBuffer::FromPath(filepath);
    constexpr auto gltf_options = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

    fastgltf::Asset gltf;
    fastgltf::Parser parser{};

    auto load = parser.loadGltfBinary(data.get(), filepath.parent_path(), gltf_options);
    if (load)
        gltf = std::move(load.get());
    else {
        std::cout << "| ERROR: Failed to load gltf: " << fastgltf::to_underlying(load.error()) << "\n";
        return {};
    }

    std::vector<std::shared_ptr<MeshAsset>> meshes;

    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;
    for (fastgltf::Mesh& mesh : gltf.meshes) {
        MeshAsset engine_mesh;
        engine_mesh.name = mesh.name;

        indices.clear();
        vertices.clear();

        for (auto&& p : mesh.primitives) {
            GeoSurface new_surface;
            new_surface.start_idx = static_cast<uint32_t>(indices.size());
            new_surface.count = static_cast<uint32_t>(gltf.accessors[p.indicesAccessor.value()].count);
            size_t initial_vtx = vertices.size();

            { // Indices
                fastgltf::Accessor& index_accessor = gltf.accessors[p.indicesAccessor.value()];
                indices.reserve(indices.size() + index_accessor.count);
                fastgltf::iterateAccessor<std::uint32_t>(gltf, index_accessor,
                    [&](std::uint32_t idx) {
                        indices.push_back(idx + initial_vtx);
                    });
            }
            { // Vertex positions
                fastgltf::Accessor& pos_accessor = gltf.accessors[p.findAttribute("POSITION")->accessorIndex];
                vertices.resize(vertices.size() + pos_accessor.count);

                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, pos_accessor,
                    [&](glm::vec3 v, size_t index) {
                        Vertex newvtx;
                        newvtx.position = v;
                        newvtx.normal = { 1, 0, 0 };
                        newvtx.color = glm::vec4{ 1.f };
                        newvtx.uv_x = 0;
                        newvtx.uv_y = 0;
                        vertices[initial_vtx + index] = newvtx;
                    });
            }
            // load vertex normals
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).accessorIndex],
                    [&](glm::vec3 v, size_t index) {
                        vertices[initial_vtx + index].normal = v;
                    });
            }
            // load UVs
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).accessorIndex],
                    [&](glm::vec2 v, size_t index) {
                        vertices[initial_vtx + index].uv_x = v.x;
                        vertices[initial_vtx + index].uv_y = v.y;
                    });
            }
            // load vertex colors
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end()) {

                fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).accessorIndex],
                    [&](glm::vec4 v, size_t index) {
                        vertices[initial_vtx + index].color = v;
                    });
            }
            engine_mesh.surfaces.push_back(new_surface);
        }
        // display the vertex normals
        constexpr bool OverrideColors = true;
        if (OverrideColors) {
            for (Vertex& vtx : vertices) {
                vtx.color = glm::vec4(vtx.normal, 1.f);
            }
        }
        engine_mesh.mesh_buffers = vk_context->upload_mesh(indices, vertices);
        meshes.emplace_back(std::make_shared<MeshAsset>(std::move(engine_mesh)));
    }
    return meshes;
}