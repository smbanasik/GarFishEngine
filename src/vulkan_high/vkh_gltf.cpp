// Spencer Banasik
// Created: 12/29/2024
// Last Modified: 12/29/2024
#include <vkh_gltf.hpp>

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <gf_vulkan.hpp>
#include <vkl_initializers.hpp>
#include <vkl_types.hpp>
#include <vkl_imgbuf_alloc.hpp>
#include <mat_metrough.hpp>

std::optional<std::shared_ptr<gf::vk_loader::LoadedGLTF>> gf::vk_loader::load_gltf(VkManager* engine, std::string_view file_path) {
    
    std::cout << "| INFO: loading GLTF: " << file_path << "\n";

    std::shared_ptr<LoadedGLTF> scene = std::make_shared<LoadedGLTF>();
    scene->creator = engine;
    LoadedGLTF& file = *scene.get();
    constexpr auto gltf_options = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

    fastgltf::Asset gltf;
    fastgltf::Parser parser{};
    std::filesystem::path path = file_path;

    auto data = fastgltf::GltfDataBuffer::FromPath(file_path);
    auto type = fastgltf::determineGltfFileType(data.get());

    if (type == fastgltf::GltfType::glTF) {
        auto load = parser.loadGltf(data.get(), path.parent_path(), gltf_options);
        if (load)
            gltf = std::move(load.get());
        else {
            std::cout << "| ERROR: Failed to load gltf: " << fastgltf::to_underlying(load.error()) << "\n";
            return {};
        }
    }
    else if (type == fastgltf::GltfType::GLB) {
        auto load = parser.loadGltfBinary(data.get(), path.parent_path(), gltf_options);
        if (load)
            gltf = std::move(load.get());
        else {
            std::cout << "| ERROR: Failed to load gltf: " << fastgltf::to_underlying(load.error()) << "\n";
            return {};
        }
    }
    else {
        std::cout << "| ERROR: could not determine gltf container.\n";
        return {};
    }

    std::vector<vkl_desc::DescriptorAllocatorGrowable::PoolSizeRatio> sizes = { { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 } };

    file.descriptor_pool.init(engine->core.device, gltf.materials.size(), sizes);
    
    for (fastgltf::Sampler& sampler : gltf.samplers) {

        VkSamplerCreateInfo sampl = { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, .pNext = nullptr };
        sampl.maxLod = VK_LOD_CLAMP_NONE;
        sampl.minLod = 0;
        sampl.magFilter = extract_filter(sampler.magFilter.value_or(fastgltf::Filter::Nearest));
        sampl.minFilter = extract_filter(sampler.minFilter.value_or(fastgltf::Filter::Nearest));
        sampl.mipmapMode = extract_mipmap_mode(sampler.minFilter.value_or(fastgltf::Filter::Nearest));

        VkSampler new_sampler;
        vkCreateSampler(engine->core.device, &sampl, nullptr, &new_sampler);

        file.samplers.push_back(new_sampler);
    }

    std::vector<std::shared_ptr<vkh_render::Node>> nodes;
    std::vector<std::shared_ptr<vkh_render::MeshAsset>> meshes;
    std::vector<std::shared_ptr<vkh_render::GLTFMaterial>> materials;
    std::vector<vkl_res::AllocatedImage> images;

    for (fastgltf::Image& image : gltf.images) {
        std::optional<vkl_res::AllocatedImage> img = load_image(&engine->img_buff_allocator, gltf, image);

        if (img.has_value()) {
            images.push_back(*img);
            file.images[image.name.c_str()] = *img;
        }
        else {
            images.push_back(*engine->engine_images.get_texture("error_checkerboard"));
            std::cout << "gltf failed to load texture " << image.name << std::endl;
        }
    }

    file.material_data_buffer = engine->img_buff_allocator.create_buffer(sizeof(vkh_mat::GLTFMetallic_Roughness::MaterialConstants) * gltf.materials.size(),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    int data_index = 0;
    vkh_mat::GLTFMetallic_Roughness::MaterialConstants* scene_material_constants = (vkh_mat::GLTFMetallic_Roughness::MaterialConstants*)file.material_data_buffer.info.pMappedData;

    for (fastgltf::Material& mat : gltf.materials) {
        std::shared_ptr<vkh_render::GLTFMaterial> new_mat = std::make_shared<vkh_render::GLTFMaterial>();
        materials.push_back(new_mat);
        file.materials[mat.name.c_str()] = new_mat;

        vkh_mat::GLTFMetallic_Roughness::MaterialConstants constants;
        constants.color_factors.x = mat.pbrData.baseColorFactor[0];
        constants.color_factors.y = mat.pbrData.baseColorFactor[1];
        constants.color_factors.z = mat.pbrData.baseColorFactor[2];
        constants.color_factors.w = mat.pbrData.baseColorFactor[3];

        constants.metal_rough_factors.x = mat.pbrData.metallicFactor;
        constants.metal_rough_factors.y = mat.pbrData.roughnessFactor;
        scene_material_constants[data_index] = constants;

        vkl::MaterialPass passType = vkl::MaterialPass::MainColor;
        if (mat.alphaMode == fastgltf::AlphaMode::Blend) {
            passType = vkl::MaterialPass::Transparent;
        }

        vkh_mat::GLTFMetallic_Roughness::MaterialResources material_resources(engine->img_buff_allocator);
        material_resources.color_image = *engine->engine_images.get_texture("white");
        material_resources.color_sampler = engine->default_sampler_linear;
        material_resources.metal_rough_image = *engine->engine_images.get_texture("white");
        material_resources.metal_rough_sampler = engine->default_sampler_linear;

        material_resources.data_buffer = file.material_data_buffer.buffer;
        material_resources.data_buffer_offset = data_index * sizeof(vkh_mat::GLTFMetallic_Roughness::MaterialConstants);
        if (mat.pbrData.baseColorTexture.has_value()) {
            size_t img = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].imageIndex.value();
            size_t sampler = gltf.textures[mat.pbrData.baseColorTexture.value().textureIndex].samplerIndex.value();

            material_resources.color_image = images[img];
            material_resources.color_sampler = file.samplers[sampler];
        }
        new_mat->data = engine->mat_manager.get_material("metal_mat")->write_material(engine->core.device, passType, material_resources, file.descriptor_pool);

        data_index++;
    }
    std::vector<uint32_t> indices;
    std::vector<vkl::Vertex> vertices;

    for (fastgltf::Mesh& mesh : gltf.meshes) {
        std::shared_ptr<vkh_render::MeshAsset> newmesh = std::make_shared<vkh_render::MeshAsset>();
        meshes.push_back(newmesh);
        file.meshes[mesh.name.c_str()] = newmesh;
        newmesh->name = mesh.name;

        indices.clear();
        vertices.clear();

        for (auto&& p : mesh.primitives) {
            vkh_render::GeoSurface newSurface;
            newSurface.start_idx = (uint32_t)indices.size();
            newSurface.count = (uint32_t)gltf.accessors[p.indicesAccessor.value()].count;
            size_t initial_vtx = vertices.size();
            {
                fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
                indices.reserve(indices.size() + indexaccessor.count);

                fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor,
                    [&](std::uint32_t idx) {
                        indices.push_back(idx + initial_vtx);
                    });
            }
            {
                fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->accessorIndex];
                vertices.resize(vertices.size() + posAccessor.count);

                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
                    [&](glm::vec3 v, size_t index) {
                        vkl::Vertex newvtx;
                        newvtx.position = v;
                        newvtx.normal = { 1, 0, 0 };
                        newvtx.color = glm::vec4{ 1.f };
                        newvtx.uv_x = 0;
                        newvtx.uv_y = 0;
                        vertices[initial_vtx + index] = newvtx;
                    });
            }
            auto normals = p.findAttribute("NORMAL");
            if (normals != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).accessorIndex],
                    [&](glm::vec3 v, size_t index) {
                        vertices[initial_vtx + index].normal = v;
                    });
            }
            auto uv = p.findAttribute("TEXCOORD_0");
            if (uv != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).accessorIndex],
                    [&](glm::vec2 v, size_t index) {
                        vertices[initial_vtx + index].uv_x = v.x;
                        vertices[initial_vtx + index].uv_y = v.y;
                    });
            }
            auto colors = p.findAttribute("COLOR_0");
            if (colors != p.attributes.end()) {
                fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).accessorIndex],
                    [&](glm::vec4 v, size_t index) {
                        vertices[initial_vtx + index].color = v;
                    });
            }
            if (p.materialIndex.has_value()) {
                newSurface.material = materials[p.materialIndex.value()];
            }
            else {
                newSurface.material = materials[0];
            }
            glm::vec3 minpos = vertices[initial_vtx].position;
            glm::vec3 maxpos = vertices[initial_vtx].position;
            for (int i = initial_vtx; i < vertices.size(); i++) {
                minpos = glm::min(minpos, vertices[i].position);
                maxpos = glm::max(maxpos, vertices[i].position);
            }
            // calculate origin and extents from the min/max, use extent lenght for radius
            newSurface.bounds.origin = (maxpos + minpos) / 2.f;
            newSurface.bounds.extents = (maxpos - minpos) / 2.f;
            newSurface.bounds.sphereRadius = glm::length(newSurface.bounds.extents);

            newmesh->surfaces.push_back(newSurface);
        }
        newmesh->mesh_buffers = engine->upload_mesh(indices, vertices);
    }
    for (fastgltf::Node& node : gltf.nodes) {
        std::shared_ptr<vkh_render::Node> newNode;
        if (node.meshIndex.has_value()) {
            newNode = std::make_shared<vkh_render::MeshNode>();
            static_cast<vkh_render::MeshNode*>(newNode.get())->mesh = meshes[*node.meshIndex];
        }
        else {
            newNode = std::make_shared<vkh_render::Node>();
        }

        nodes.push_back(newNode);
        file.nodes[node.name.c_str()];

        std::visit(
            fastgltf::visitor{ 
                [&](fastgltf::math::fmat4x4 matrix) {
                    memcpy(&newNode->local_transform, matrix.data(), sizeof(matrix));
                },
                [&](fastgltf::TRS transform) {
                    glm::vec3 tl(transform.translation[0], transform.translation[1],
                        transform.translation[2]);
                    glm::quat rot(transform.rotation[3], transform.rotation[0], transform.rotation[1],
                        transform.rotation[2]);
                    glm::vec3 sc(transform.scale[0], transform.scale[1], transform.scale[2]);

                    glm::mat4 tm = glm::translate(glm::mat4(1.f), tl);
                    glm::mat4 rm = glm::toMat4(rot);
                    glm::mat4 sm = glm::scale(glm::mat4(1.f), sc);

                    newNode->local_transform = tm * rm * sm;
                } 
            },
            node.transform);
    }

    for (int i = 0; i < gltf.nodes.size(); i++) {
        fastgltf::Node& node = gltf.nodes[i];
        std::shared_ptr<vkh_render::Node>& sceneNode = nodes[i];

        for (auto& c : node.children) {
            sceneNode->children.push_back(nodes[c]);
            nodes[c]->parent = sceneNode;
        }
    }

    for (auto& node : nodes) {
        if (node->parent.lock() == nullptr) {
            file.top_nodes.push_back(node);
            node->refresh_transform(glm::mat4{ 1.f });
        }
    }
    return scene;
}

VkFilter gf::vk_loader::extract_filter(fastgltf::Filter filter) {
    switch (filter) {
    case fastgltf::Filter::Nearest:
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::NearestMipMapLinear:
        return VK_FILTER_NEAREST;

    case fastgltf::Filter::Linear:
    case fastgltf::Filter::LinearMipMapNearest:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
        return VK_FILTER_LINEAR;
    }
}
VkSamplerMipmapMode gf::vk_loader::extract_mipmap_mode(fastgltf::Filter filter) {
    switch (filter) {
    case fastgltf::Filter::NearestMipMapNearest:
    case fastgltf::Filter::LinearMipMapNearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case fastgltf::Filter::NearestMipMapLinear:
    case fastgltf::Filter::LinearMipMapLinear:
    default:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

void gf::vk_loader::LoadedGLTF::draw(const glm::mat4& top_matrix, vkl::DrawContext& ctx) {
    for (auto& n : top_nodes) {
        n->draw(top_matrix, ctx);
    }

}
void gf::vk_loader::LoadedGLTF::clear_all() {
        VkDevice dv = creator->core.device;

        descriptor_pool.destroy_pools(dv);

        meshes.clear();
        images.clear();

        for (auto& sampler : samplers) {
            vkDestroySampler(dv, sampler, nullptr);
        }
}
std::optional<vkl_res::AllocatedImage> gf::vk_loader::load_image(vkl_res::ImageBufferAllocator* allocator, fastgltf::Asset& asset, fastgltf::Image& image) {
    vkl_res::AllocatedImage newImage(*allocator);

    int width, height, nrChannels;

    std::visit(
        fastgltf::visitor{
            [&](fastgltf::sources::URI& filePath) {
                assert(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
                assert(filePath.uri.isLocalPath()); // We're only capable of loading
                // local files.

                const std::string path(filePath.uri.path().begin(),
                    filePath.uri.path().end()); // Thanks C++.
                unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
                if (data) {
                    VkExtent3D imagesize;
                    imagesize.width = width;
                    imagesize.height = height;
                    imagesize.depth = 1;

                    newImage = allocator->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT,false);

                    stbi_image_free(data);
                }
            },
            [&](fastgltf::sources::Array& array) {
                unsigned char* data = stbi_load_from_memory((unsigned char*)array.bytes.data(), static_cast<int>(array.bytes.size()),
                    &width, &height, &nrChannels, 4);
                if (data) {
                    VkExtent3D imagesize;
                    imagesize.width = width;
                    imagesize.height = height;
                    imagesize.depth = 1;

                    newImage = allocator->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT,false);

                    stbi_image_free(data);
                }
            },
            [&](fastgltf::sources::BufferView& view) {
                auto& bufferView = asset.bufferViews[view.bufferViewIndex];
                auto& buffer = asset.buffers[bufferView.bufferIndex];

                std::visit(fastgltf::visitor { // We only care about VectorWithMime here, because we
                    // specify LoadExternalBuffers, meaning all buffers
                    // are already loaded into a vector
                [&](fastgltf::sources::Array& array) {
                    unsigned char* data = stbi_load_from_memory((unsigned char*)array.bytes.data() + bufferView.byteOffset,
                        static_cast<int>(bufferView.byteLength),
                        &width, &height, &nrChannels, 4);
                    if (data) {
                        VkExtent3D imagesize;
                        imagesize.width = width;
                        imagesize.height = height;
                        imagesize.depth = 1;

                        newImage = allocator->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM,
                            VK_IMAGE_USAGE_SAMPLED_BIT,false);

                        stbi_image_free(data);
                    }
                },
                [](auto& arg) {},
                },
                buffer.data);
            },
            [](auto& arg) {},
        },
        image.data);

    // if any of the attempts to load the data failed, we havent written the image
    // so handle is null
    if (newImage.image == VK_NULL_HANDLE) {
        return std::nullopt;
    }
    else {
        return newImage;
    }
}