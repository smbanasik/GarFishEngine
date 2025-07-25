/**
* @file
* @brief Vulkan entrypoint for engine.
* @author Spencer Banasik
*/
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

#include <gf_stats.hpp>
#include <t_delstack.hpp>
#include <vkl_types.hpp>
#include <vkl_core.hpp>
#include <vkl_alloc.hpp>
#include <t_desc_allocgrow.hpp>
#include <vkl_pipelines.hpp>
#include <vkh_gltf.hpp>
#include <t_imageatlas.hpp>
#include <vkl_swapchain.hpp>
#include <vkl_framedata.hpp>
#include <vkl_imm_frame.hpp>
#include <vkl_images.hpp>
#include <gfu_types.hpp>
#include <gf_resource_manager.hpp>
#include <gf_text.hpp>
#include <vkl_imgbuf_alloc.hpp>
#include <vkl_mat_types.hpp>
#include <vkl_shader_types.hpp>
#include <vkh_material_manager.hpp>

struct GLFWwindow;

namespace gf {
namespace wi {
class WIManager;
class WInputContext;
struct Key;
}

static constexpr bool DEBUG_USE_VALIDATION = true; /**< Whether to use validation layers or not */
constexpr uint8_t FRAME_OVERLAP = 2;

/**
 * @struct Camera
 * @brief Temporary structure to handle camera.
 */
struct Camera {
    // Very hacky and awful but I don't care!
    // I will be reworking this shortly
    static glm::vec3 velocity;
    static glm::vec3 position;
    static float pitch;
    static float yaw;
    static bool mouse_enabled;

    glm::mat4 get_view_matrix();
    glm::mat4 get_rotation_matrix();

    static void glfw_camera_callback(wi::WInputContext* context, wi::Key* key);
    static void glfw_camera_mouse(wi::WInputContext* context);
    static void mouse_swap(wi::WInputContext* context, wi::Key* key);

    void update();
};

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
    vkl_frames::SwapChain swapchain;
    vkl_frames::FrameData frame_data;
    vkl_frames::ImmediateFrame imm_frame;
    vkl_res::ImageBufferAllocator img_buff_allocator;
    vkl_desc::DescriptorAllocatorGrowable global_descriptor_allocator;
    gf::EngineStats stats;
    
    VkPipeline gradient_pipeline;
    VkPipelineLayout gradient_pipeline_layout;

    /**
     * @brief Image we draw to
     * @todo rename to draw_image
     */
    vkl_res::AllocatedImage drawn_image;
    vkl_res::AllocatedImage depth_image;
    VkExtent2D drawn_size;
    float render_scale = 1.f;
    
    VkDescriptorSet drawn_image_descriptors;
    VkDescriptorSetLayout drawn_image_descriptor_layout;
    vkl::GPUSceneData scene_data;
    VkDescriptorSetLayout gpu_scene_data_descriptor_layout;
    VkDescriptorSetLayout single_image_descriptor_layout;

    ImageManager engine_images;
    text::TextManager text_manager;
    VkSampler default_sampler_linear;
    VkSampler default_sampler_nearest;

    vkl::MaterialInstance default_data;
    vkh_mat::MaterialManager mat_manager;
    vkl::MaterialInstance image_mat_data;
    vkh::ImageAtlas test_texture;

    vkl::DrawContext main_draw_context;
    std::unordered_map<std::string, std::shared_ptr<vkh_render::Node>> loaded_nodes;
    std::unordered_map<std::string, std::shared_ptr<vk_loader::LoadedGLTF>> loaded_scenes;
    Camera camera;

    std::vector<std::shared_ptr<vkh_render::MeshAsset>> test_meshes;
    std::vector<vkl::ComputeEffect> background_effects; // For fun!
    int current_background_effect{ 0 }; // For fun!

    bool is_init = false;
    bool resize_requested = false;

    /**
     * @brief Constructs the VKManger
     * @author Spencer Banasik
     * @param [in] gl_manager Glfw manager to create a surface.
     * @param [in] gl_context Provides the window to attach the surface to.
     * @todo Change the surface and context setup to not require the manager.
     */
    VkManager(wi::WIManager& wi_manager, wi::WInputContext& wi_context);
    
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
    void update_scene(uint32_t width, uint32_t height);
    
    /**
     * @brief Draws the backgrounds with the compute pipelines
     * @param [in, out] cmd Buffer that we push draw commands to.
     * @param [in] clear Clear color, not used.
     */
    void draw_background(VkCommandBuffer cmd, VkClearColorValue& clear);

    /**
     * @brief Draws the geometry from the draw contexts.
     * @param [in, out] cmd Command buffer we push draw commands to.
     * @param [in, out] frame Descriptors and deletion info concerning the frame.
     */
    void draw_geometry(VkCommandBuffer cmd, vkl_frames::Frame* frame);

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
    com::DeletionStack global_deletion_stack;

    VkManager(const VkManager& other) = delete;
    VkManager& operator=(const VkManager& other) = delete;

    /**
     * @brief Does not create teh swap chain anymore, instead sets up images for swapchain.
     * @param [in] width Screen width.
     * @param [in] height Screen height.
     * @details Produces the draw image and depth image for the engine.
     * @todo Remove or, if we can't remove, rename this.
     */
    void init_swapchain(uint32_t width, uint32_t height);

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
     * @brief Initialize IMGUI
     * @param [in] window Windo that IMGUI is attached to.
     */
    void init_imgui(GLFWwindow* window);

    /**
     * @brief Initialize default colors and textures, alongside some test code.
     */
    void init_default_data();
};
}
#endif
