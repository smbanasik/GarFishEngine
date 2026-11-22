#include <vulkan_high/graphics_system.hpp>

#include <chrono>

#include <ft2build.h>
#include FT_FREETYPE_H
#define VMA_IMPLEMENTATION

#include <window_input/wi_manager.hpp>

static FT_Library ft_lib;

gs::GraphicsSystem::GraphicsSystem(const com::PathFinder& path_finder, gf::wi::WIManager& wi_manager,
                                   gf::wi::WInputContext& wi_context)
    : stats(), graphics_context(path_finder, wi_manager, wi_context), image_manager(graphics_context.gpu_allocator),
      font_manager(graphics_context.gpu_allocator) /*, mat_manager()*/, mesh_manager(),
      default_data(graphics_context.core.device, image_manager),
      renderer(graphics_context, wi_context.window.get_window_dims()) {

    // TODO: keep disabled while we are using vulkan.hpp
    // FT_Error err = FT_Init_FreeType(&ft_lib);
    // assert(err == 0);
    // vkh_font_manager::FontManager::ft_lib = &ft_lib;

    renderer.temp_init_debug_ui(graphics_context, wi_manager, wi_context);

    is_init = true;
}

gs::GraphicsSystem::~GraphicsSystem() {
    vkDeviceWaitIdle(graphics_context.core.device);

    graphics_context.global_deletion_stack.flush();
}

void gs::GraphicsSystem::draw_step(gf::wi::Extent2D dims) {

    auto start = std::chrono::system_clock::now();

    if (!should_render)
        return;

    if (renderer.is_window_resize_requested()) {
        renderer.resize_swapchain(graphics_context.core.device, dims);
    }

    renderer.render_frame(graphics_context);

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    stats.frametime = elapsed.count() / 1000.f;
}