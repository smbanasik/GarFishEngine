#include <vulkan_high/graphics_system.hpp>
#include <window_input/wi_manager.hpp>

gs::GraphicsSystem::GraphicsSystem(const com::PathFinder& path_finder, gf::wi::WIManager& wi_manager,
                                   gf::wi::WInputContext& wi_context)
    : stats(), graphics_context(path_finder, wi_manager, wi_context), image_manager(graphics_context.gpu_allocator),
      font_manager(graphics_context.gpu_allocator) /*, mat_manager()*/, mesh_manager(),
      default_data(graphics_context.core.device, image_manager),
      renderer(graphics_context, wi_context.window.get_window_dims()) {
    is_init = true;
}

gs::GraphicsSystem::~GraphicsSystem() {
    vkDeviceWaitIdle(graphics_context.core.device);

    graphics_context.global_deletion_stack.flush();
}