/**
 * @file
 * @brief Vulkan entrypoint for engine, holds everything to render graphics
 * @author Spencer Banasik
 */
#ifndef GS_GRAPHICS_SYSTEM_HPP
#define GS_GRAPHICS_SYSTEM_HPP

#include <common/paths.hpp>
#include <garfish/stats.hpp>
#include <vulkan_high/default_data.hpp>
#include <vulkan_high/graphics_context.hpp>
#include <vulkan_high/image_manager.hpp>
#include <vulkan_high/materials/material_manager.hpp>
#include <vulkan_high/mesh_manager.hpp>
#include <vulkan_high/renderer.hpp>
#include <vulkan_text/font_manager.hpp>

namespace gf {
namespace wi {
class WIManager;
class WInputContext;
} // namespace wi
} // namespace gf

namespace gs {
/**
 * @class GraphicsSystem
 * @brief The main vulkan manager.
 * @details This class owns all of the abstract vulkan constructs and
 * initializes the vulkan library. All of vulkan's setup and concepts are here.
 * @pre Requires a setup WInputContext to function.
 */
class GraphicsSystem {
  public:
    GraphicsSystem(const com::PathFinder& path_finder, gf::wi::WIManager& wi_manager,
                   gf::wi::WInputContext& wi_context);
    ~GraphicsSystem();
    GraphicsSystem(const GraphicsSystem& other) = delete;
    GraphicsSystem& operator=(const GraphicsSystem& other) = delete;

    bool is_init = false;
    bool window_resize_requested = false;
    gf::EngineStats stats; // TODO: rename to renderer stats

    GraphicsContext graphics_context;
    vkh::ImageManager image_manager;
    vkh_font_manager::FontManager font_manager;
    // vkh_mat::MaterialManager
    //     mat_manager; // TODO: oops! this requires the vk_manager thrown into it. We're coming back to this.
    vkh::MeshManager mesh_manager;

    gs::DefaultData default_data;
    gs::Renderer renderer;
};

} // namespace gs
#endif