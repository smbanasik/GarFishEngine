#include <vulkan_high/renderer.hpp>

gs::Renderer::Renderer(gs::GraphicsContext& graphics_context, gf::wi::Extent2D dims)
    : swapchain(&graphics_context.core, dims.width, dims.height), frame_data(&graphics_context.core),
      frame_renderer(graphics_context, dims) {}

bool gs::Renderer::resize_swapchain(VkDevice device, gf::wi::Extent2D dims) {

    if (dims.width == 0 || dims.height == 0)
        return false;

    vkDeviceWaitIdle(device);

    swapchain.destroy_swapchain();
    swapchain.remake_swapchain(dims.width, dims.height);

    return true;
}