// Spencer Banasik
// Created: 12/16/2024
// Last Modified: 1/7/2025
// Description:
// Handles abstract and low level vulkan concepts
#ifndef GF_VULKAN_HPP
#define GF_VULKAN_HPP

#include <vector>
#include <string>
#include <stdint.h>
#include <array>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <engine_types.hpp>
#include <vk_types.hpp>
#include <vk_core.hpp>
#include <vk_descriptors.hpp>
#include <vk_pipelines.hpp>
#include <vk_loader.hpp>
#include <vk_materials.hpp>
#include <vk_renderable.hpp>
#include <vk_frames.hpp>
#include <vk_images.hpp>

struct GLFWwindow;

namespace gf {
namespace gl {
class GLManager;
class WInputContext;
struct Key;
}
static constexpr bool DEBUG_USE_VALIDATION = true;
constexpr uint8_t FRAME_OVERLAP = 2;

struct Camera {
    // Very hacky and awful but I don't care!
    // I will be reworking this shortly
    static glm::vec3 velocity;
    static glm::vec3 position;
    static float pitch;
    static float yaw;

    glm::mat4 get_view_matrix();
    glm::mat4 get_rotation_matrix();

    static void glfw_camera_callback(gl::Key* key);
    static void glfw_camera_mouse(gl::WInputContext* context);

    void update();
};

// The VkManager owns all of the abstract vulkan constructs and handles the
// initialization of the vulkan library
class VkManager {
public:

    vk_core::VKCore core;
    vk_core::Alloc alloc;
    vk_frames::SwapChain swapchain;
    vk_frames::FrameData frame_data;
    vk_frames::ImmediateFrame imm_frame;
    vk_img::ImageBufferAllocator img_buff_allocator;
    vk_desc::DescriptorAllocatorGrowable global_descriptor_allocator;
    
    VkPipeline gradient_pipeline;
    VkPipelineLayout gradient_pipeline_layout;
    MaterialPipeline triangle_pipeline;
    MaterialPipeline mesh_pipeline;

    AllocatedImage drawn_image;
    AllocatedImage depth_image;
    VkExtent2D drawn_size;
    float render_scale = 1.f;
    
    VkDescriptorSet drawn_image_descriptors;
    VkDescriptorSetLayout drawn_image_descriptor_layout;
    GPUSceneData scene_data;
    VkDescriptorSetLayout gpu_scene_data_descriptor_layout;
    VkDescriptorSetLayout single_image_descriptor_layout;

    AllocatedImage white_image;
    AllocatedImage black_image;
    AllocatedImage gray_image;
    AllocatedImage error_checkerboard_image;
    VkSampler default_sampler_linear;
    VkSampler default_sampler_nearest;

    MaterialInstance default_data;
    vk_mat::GLTFMetallic_Roughness metal_rough_material;

    vk_render::DrawContext main_draw_context;
    std::unordered_map<std::string, std::shared_ptr<vk_render::Node>> loaded_nodes;
    std::unordered_map<std::string, std::shared_ptr<vk_loader::LoadedGLTF>> loaded_scenes;
    Camera camera;

    std::vector<std::shared_ptr<gf::vk_render::MeshAsset>> test_meshes;
    std::vector<ComputeEffect> background_effects; // For fun!
    int current_background_effect{ 0 }; // For fun!

    bool is_init = false;
    bool resize_requested = false;

    VkManager(gl::GLManager& gl_manager, gl::WInputContext& gl_context);
    ~VkManager();

    VkManager& get();

    void update_scene(uint32_t width, uint32_t height);

    void draw_background(VkCommandBuffer cmd, VkClearColorValue& clear);
    void draw_geometry(VkCommandBuffer cmd, Frame* frame);

    GPUMeshBuffers upload_mesh(std::span<uint32_t> indices, std::span<Vertex> vertices);
    void resize_swapchain(uint32_t width, uint32_t height);

private:
    static VkManager* loaded_vk;
    DeletionStack global_deletion_stack;

    VkManager(const VkManager& other) = delete;
    VkManager& operator=(const VkManager& other) = delete;
    void init_swapchain(uint32_t width, uint32_t height);
    void init_descriptors();
    void init_pipelines();
    void init_background_pipelines();
    void init_triangle_pipeline();
    void init_mesh_pipeline();
    void init_imgui(GLFWwindow* window);
    void init_default_data();
};
}
#endif
