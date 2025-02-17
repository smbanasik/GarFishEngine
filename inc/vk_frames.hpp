// Spencer Banasik
// Created: 1/7/2024
// Last Modified: 1/7/2025
// Description:
// The rendering setup for frames goes here
#ifndef VK_FRAMES_HPP
#define VK_FRAMES_HPP

#include <array>
#include <memory>

#include <vulkan/vulkan.h>

#include <engine_types.hpp>
#include <vk_descriptors.hpp>
#include <vk_types.hpp>

namespace gf {
namespace vk_core {
class VKCore;
}
namespace vk_frames {
constexpr uint8_t FRAME_OVERLAP = 2;
// A RAII structure of a swapchain with the ability to resize it as needed.
// Can only be moved, not copied.
class SwapChain {
public:

    SwapChain(vk_core::VKCore* core, uint32_t width, uint32_t height);
    ~SwapChain();
    SwapChain(SwapChain&& other) noexcept;
    SwapChain& operator=(SwapChain&& other) noexcept {
        swapchain = std::move(other.swapchain);
        swapchain_format = std::move(other.swapchain_format);
        swapchain_images = std::move(other.swapchain_images);
        swapchain_image_views = std::move(other.swapchain_image_views);
        core_handle = std::move(other.core_handle);
        other.swapchain = nullptr;
        for (auto it = other.swapchain_image_views.begin(); it != swapchain_image_views.end(); it++) {
            *it = nullptr;
        }
    }
    SwapChain(SwapChain& other) = delete;
    SwapChain& operator=(SwapChain& other) = delete;
    
    void remake_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();

    VkSwapchainKHR swapchain{};
    VkFormat swapchain_format{};
    VkExtent2D swapchain_extent{};
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
private:
    vk_core::VKCore* core_handle;
};
// A per frame RAII structure containing what's needed for a frame in the swapchain.
// Can only be moved, not copied.
class FrameData {
public:

    FrameData(vk_core::VKCore* core);
    ~FrameData();
    FrameData(FrameData&& other) noexcept;
    FrameData& operator=(FrameData&& other) noexcept {
        core_handle = std::move(other.core_handle);
        active_frames = std::move(other.active_frames);
        for (auto it = other.active_frames.begin(); it != other.active_frames.end(); it++) {
            (*it).command_pool = nullptr;
            (*it).command_buffer = nullptr;
            (*it).render_fence = nullptr;
            (*it).render_semaphore = nullptr;
            (*it).swapchain_semaphore = nullptr;
        }
    }
    FrameData(FrameData& other) = delete;
    FrameData& operator=(FrameData& other) = delete;

    std::array<Frame, FRAME_OVERLAP> active_frames;
private:
    vk_core::VKCore* core_handle;
};
// A RAII structure containing what's needed for immediate submission to the GPU
// Can only be moved, not copied.
class ImmediateFrame {
public:

    ImmediateFrame(vk_core::VKCore* core);
    ~ImmediateFrame();
    ImmediateFrame(ImmediateFrame&& other) noexcept;
    ImmediateFrame& operator=(ImmediateFrame&& other) noexcept {
        imm_fence = std::move(other.imm_fence);
        imm_command_buffer = std::move(other.imm_command_buffer);
        imm_command_pool = std::move(other.imm_command_pool);
        other.imm_command_pool = nullptr;
        other.imm_fence = nullptr;
    }
    ImmediateFrame(ImmediateFrame& other) = delete;
    ImmediateFrame& operator=(ImmediateFrame& other) = delete;

    VkFence imm_fence;
    VkCommandBuffer imm_command_buffer;
    VkCommandPool imm_command_pool;

    void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
private:
    vk_core::VKCore* core_handle;
};
// A RAII implementation of a rendering pipeline
class Renderer {
public:

    Renderer();
    ~Renderer();
    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer&& other) noexcept;
    Renderer(Renderer& other) = delete;
    Renderer& operator=(Renderer& other) = delete;

    void remake_swapchain(uint32_t width, uint32_t height);

    SwapChain swapchain;
    std::array<FrameData, FRAME_OVERLAP> active_frames;

private:
    void create_swapchain(uint32_t width, uint32_t height);
    void create_frame_data();
};

}
}
#endif
