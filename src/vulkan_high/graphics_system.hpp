/**
 * @file
 * @brief Vulkan entrypoint for engine, holds everything to render graphics
 * @author Spencer Banasik
 */
#ifndef GS_GRAPHICS_SYSTEM_HPP
#define GS_GRAPHICS_SYSTEM_HPP

#include <common/paths.hpp>
#include <garfish/stats.hpp>
#include <vulkan_high/image_manager.hpp>
#include <vulkan_high/materials/material_manager.hpp>
#include <vulkan_high/renderer.hpp>
#include <vulkan_low/graphics_context/alloc.hpp>
#include <vulkan_low/graphics_context/core.hpp>
#include <vulkan_text/font_manager.hpp>

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
    vkl_core::VKCore core;
    vkl_core::VMAAllocWrapper alloc;
    vkl_res::ImageBufferAllocator img_buff_allocator;
    gs::Renderer renderer;
    com::DeletionStack global_deletion_stack;

    vkh::ImageManager engine_images;
    vkh_font_manager::FontManager font_manager;
    vkh_mat::MaterialManager mat_manager;

    gf::EngineStats stats; // TODO: rename to renderer stats
    const com::PathFinder& path_finder;

    bool is_init = false;

  private:
    GraphicsSystem(const GraphicsSystem& other) = delete;
    GraphicsSystem& operator=(const GraphicsSystem& other) = delete;
};

} // namespace gs
#endif