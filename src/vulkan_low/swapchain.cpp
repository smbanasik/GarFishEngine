#include <vulkan_low/swapchain.hpp>

#include <VkBootstrap.h>

#include <vulkan_low/core.hpp>

vkl_frames::SwapChain::SwapChain(vkl_core::VKCore* core, uint32_t width, uint32_t height)
    : core_handle(core) {
    remake_swapchain(width, height);
}
vkl_frames::SwapChain::~SwapChain() {
    destroy_swapchain();
}
vkl_frames::SwapChain::SwapChain(SwapChain&& other) noexcept
    : swapchain(std::move(other.swapchain)), swapchain_format(std::move(other.swapchain_format)),
    swapchain_images(std::move(other.swapchain_images)), swapchain_image_views(std::move(other.swapchain_image_views)),
    core_handle(std::move(other.core_handle)) {
    other.swapchain = nullptr;
    for (auto it = other.swapchain_image_views.begin(); it != swapchain_image_views.end(); it++) {
        *it = nullptr;
    }
}

void vkl_frames::SwapChain::remake_swapchain(uint32_t width, uint32_t height) {
    vkb::SwapchainBuilder swapchain_builder{ core_handle->gpu, core_handle->device, core_handle->surface };
    swapchain_format = VK_FORMAT_B8G8R8A8_UNORM;

    vkb::Swapchain vkb_swapchain = swapchain_builder
        .set_desired_format(VkSurfaceFormatKHR{ .format = swapchain_format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(width, height)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

    swapchain_extent = vkb_swapchain.extent;
    swapchain = vkb_swapchain.swapchain;
    swapchain_images = vkb_swapchain.get_images().value();
    swapchain_image_views = vkb_swapchain.get_image_views().value();
}
void vkl_frames::SwapChain::destroy_swapchain() {
    if (swapchain != nullptr)
        vkDestroySwapchainKHR(core_handle->device, swapchain, nullptr);
    for (auto it = swapchain_image_views.begin(); it != swapchain_image_views.end(); it++) {
        if (*it != nullptr)
            vkDestroyImageView(core_handle->device, *it, nullptr);
    }
}