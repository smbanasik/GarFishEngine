/**
* @file
* @author Spencer Banasik
*/
#ifndef VKL_IMM_FRAME_HPP
#define VKL_IMM_FRAME_HPP

#include <utility>
#include <functional>

#include <vulkan/vulkan.h>

namespace vkl_core {
class VKCore;
}

namespace vkl_frames {
class ImmediateFrame {
public:

    ImmediateFrame(vkl_core::VKCore* core);
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
    vkl_core::VKCore* core_handle;
};
}
#endif
