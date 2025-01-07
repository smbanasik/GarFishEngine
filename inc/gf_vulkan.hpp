// Spencer Banasik
// Created: 12/16/2024
// Last Modified: 1/1/2025
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

struct GLFWwindow;

namespace gf {
namespace gl {
class GLManager;
class WInputContext;
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
    static float saved_x_pos;
    static float saved_y_pos;
    static float x_motion;
    static float y_motion;

    glm::mat4 get_view_matrix();
    glm::mat4 get_rotation_matrix();

    static void glfw_camera_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfw_camera_mouse(GLFWwindow* window, double xpos, double ypos);


    void update();
};

// The VkManager owns all of the abstract vulkan constructs and handles the
// initialization of the vulkan library
class VkManager {
public:

    vk_core::VKCore core;
    VmaAllocator allocator;
    SwapChain swapchain;
    std::array<FrameData, FRAME_OVERLAP> active_frames;
    VkFence imm_fence;
    VkCommandBuffer imm_command_buffer;
    VkCommandPool imm_command_pool;
    DescriptorAllocatorGrowable global_descriptor_allocator;
    
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
    Camera camera;

    std::vector<std::shared_ptr<gf::vk_loader::MeshAsset>> test_meshes;
    std::vector<ComputeEffect> background_effects; // For fun!
    int current_background_effect{ 0 }; // For fun!

    bool is_init = false;
    bool resize_requested = false;

    VkManager(gl::GLManager& gl_manager, gl::WInputContext& gl_context);
    ~VkManager();

    VkManager& get();

    void update_scene(uint32_t width, uint32_t height);

    void draw_background(VkCommandBuffer cmd, VkClearColorValue& clear);
    void draw_geometry(VkCommandBuffer cmd, FrameData* frame);

    GPUMeshBuffers upload_mesh(std::span<uint32_t> indices, std::span<Vertex> vertices);
    void resize_swapchain(uint32_t width, uint32_t height);

    AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
    AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
    void destroy_image(const AllocatedImage& img);

private:
    static VkManager* loaded_vk;
    DeletionStack global_deletion_stack;

    VkManager(const VkManager& other) = delete;
    VkManager& operator=(const VkManager& other) = delete;
    void init_vulkan(GLFWwindow* window);
    void init_swapchain(uint32_t width, uint32_t height);
    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();
    void init_commands();
    void create_framedata();
    void create_allocator();
    void init_descriptors();
    void init_pipelines();
    void init_background_pipelines();
    void init_triangle_pipeline();
    void init_mesh_pipeline();
    void init_imgui(GLFWwindow* window);
    void init_default_data();
    AllocatedBuffer create_buffer(size_t allocation_size, VkBufferUsageFlags flags, VmaMemoryUsage memory_usage);
    void destroy_buffer(const AllocatedBuffer& buffer);
    void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
};
}

#endif