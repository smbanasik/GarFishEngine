/**
 * @file
 * @brief Vulkan entrypoint for engine.
 * @author Spencer Banasik
 */
#ifndef GF_VULKAN_HPP
#define GF_VULKAN_HPP

#include <array>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <common/delstack.hpp>
#include <common/paths.hpp>
#include <garfish/camera.hpp>
#include <garfish/stats.hpp>
#include <garfish_user/types.hpp>
#include <vulkan_high/asset_loaders/gltf.hpp>
#include <vulkan_high/image_manager.hpp>
#include <vulkan_high/imageatlas.hpp>
#include <vulkan_high/materials/material_manager.hpp>
#include <vulkan_low/descriptors/desc_allocgrow.hpp>
#include <vulkan_low/framedata.hpp>
#include <vulkan_low/graphics_context/alloc.hpp>
#include <vulkan_low/graphics_context/core.hpp>
#include <vulkan_low/graphics_context/imgbuf_alloc.hpp>
#include <vulkan_low/images.hpp>
#include <vulkan_low/imm_frame.hpp>
#include <vulkan_low/mat_types.hpp>
#include <vulkan_low/pipelines.hpp>
#include <vulkan_low/shader_types.hpp>
#include <vulkan_low/swapchain.hpp>
#include <vulkan_low/types.hpp>
#include <vulkan_text/font_manager.hpp>

struct GLFWwindow;

namespace gf {
namespace wi {
class WIManager;
class WInputContext;
struct Key;
} // namespace wi

static constexpr bool DEBUG_USE_VALIDATION = true; /**< Whether to use validation layers or not */
constexpr uint8_t FRAME_OVERLAP = 2;

/**
 * @class VkManager
 * @brief The main vulkan manager.
 * @details This class owns all of the abstract vulkan constructs and
 * initializes the vulkan library. All of vulkan's setup is stored here.
 * @pre Requires a setup WInputContext to function.
 */
class VkManager {
  public:
    vkl_core::VKCore core;
    vkl_core::VMAAllocWrapper alloc;
    vkl_frames::SwapChain swapchain;                  // Renderer
    vkl_frames::FrameData frame_data;                 // Renderer
    vkl_frames::ImmediateFrame imm_frame;             // Renderer, probably?
    vkl_res::ImageBufferAllocator img_buff_allocator; // TODO: find a home
    vkl_desc::DescriptorAllocatorGrowable global_descriptor_allocator;
    gf::EngineStats stats; // TODO: rename to renderer stats
    const com::PathFinder& path_finder;

    VkPipeline gradient_pipeline;
    VkPipelineLayout gradient_pipeline_layout;

    /**
     * @brief Image we draw to
     */
    vkl_res::AllocatedImage draw_image; // TODO: sceneRenderer for now?
    vkl_res::AllocatedImage depth_image;
    VkExtent2D draw_size;
    float render_scale = 1.f;

    VkDescriptorSet draw_image_descriptors;
    VkDescriptorSetLayout draw_image_descriptor_layout;
    vkl::GPUSceneData scene_data;                           // TODO: scene
    VkDescriptorSetLayout gpu_scene_data_descriptor_layout; // TODO: scene?
    VkDescriptorSetLayout single_image_descriptor_layout;   // TODO: dead, delete.

    vkh::ImageManager engine_images;   // TODO: assets
    VkSampler default_sampler_linear;  // TODO: default resources
    VkSampler default_sampler_nearest; // TODO: ^

    vkh_mat::MaterialManager mat_manager; // TODO: renderer?
    vkl::MaterialInstance image_mat_data;
    vkh::ImageAtlas test_texture;
    vkh_font_manager::FontManager font_manager;

    vkl::DrawContext main_draw_context;
    std::unordered_map<std::string,
                       std::shared_ptr<vkh_render::Node>> loaded_nodes; // TODO: asset manager
    std::unordered_map<std::string,
                       std::shared_ptr<vk_loader::LoadedGLTF>> loaded_scenes; // TODO: asset manager

    Camera camera; // TODO: scene?

    std::vector<vkl::ComputeEffect> background_effects; // For fun!
    int current_background_effect{0};                   // For fun!

    bool is_init = false;
    bool resize_requested = false;

    com::DeletionStack global_deletion_stack;

    /**
     * @brief Constructs the VKManger
     * @author Spencer Banasik
     * @param [in] gl_manager Glfw manager to create a surface.
     * @param [in] gl_context Provides the window to attach the surface to.
     * @todo Change the surface and context setup to not require the manager.
     */
    VkManager(wi::WIManager& wi_manager, wi::WInputContext& wi_context, const com::PathFinder& path_finder);

    ~VkManager();

    /**
     * @brief Singleton get function.
     * @returns The singleton object.
     */
    static VkManager& get();

    /**
     * @brief Update our screen.
     * @param [in] width Screen width for camera.
     * @param [in] height Screen height for camera.
     * @details Updates the camera, clears draw contexts, and draws
     * the scenes (preparing the draw contexts)
     * Then, updates the GPUSceneData parameters.
     */
    void update_scene(uint32_t width, uint32_t height); // TODO: scene renderer

    /**
     * @brief Draws the backgrounds with the compute pipelines
     * @param [in, out] cmd Buffer that we push draw commands to.
     * @param [in] clear Clear color, not used.
     */
    void draw_background(VkCommandBuffer cmd);

    /**
     * @brief Draws the geometry from the draw contexts.
     * @param [in, out] cmd Command buffer we push draw commands to.
     * @param [in, out] frame Descriptors and deletion info concerning the frame.
     */
    void draw_geometry(VkCommandBuffer cmd, vkl_frames::Frame* frame); // TODO: scene renderer

    /**
     * @brief Given some indices and vertices, create a GPUMeshBuffer.
     * @param [in] indices The indices to use for index buffer.
     * @param [in] vertices The vertex data.
     * @returns The buffers with our mesh data, now uploaded to the GPU.
     * @todo Move this function to a more fitting class.
     */
    vkl::GPUMeshBuffers upload_mesh(std::span<uint32_t> indices, std::span<vkl::Vertex> vertices);

    /**
     * @brief Update the swapchain.
     * @param [in] width New width.
     * @param [in] height New height.
     */
    void resize_swapchain(uint32_t width, uint32_t height);

  private:
    static VkManager* loaded_vk;

    VkManager(const VkManager& other) = delete;
    VkManager& operator=(const VkManager& other) = delete;

    /**
     * @brief Does not create the swap chain anymore, instead sets up images for swapchain.
     * @param [in] width Screen width.
     * @param [in] height Screen height.
     * @details Produces the draw image and depth image for the engine.
     * @todo Remove or, if we can't remove, rename this.
     */
    void init_swapchain(uint32_t width, uint32_t height); // TODO: draw/render targets, rename.

    /**
     * @brief Sets up descriptors.
     * @detailsInitializes descriptor allocators and produces descriptors for the
     * draw image, gpu_scene_data, and the single_image_descriptor
     */
    void init_descriptors();

    /**
     * @brief Initializes pipelines with the create_material calls.
     */
    void init_pipelines();

    /**
     * @brief Creates compute pipelines for background using old system.
     * @todo Update this function and streamline compute pipeline creation.
     */
    void init_background_pipelines();

    /**
     * @brief Initialize default colors and textures, alongside some test code.
     */
    void init_default_data();
};
} // namespace gf
#endif
