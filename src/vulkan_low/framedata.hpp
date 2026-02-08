/**
* @file
* @author Spencer Banasik
*/
#ifndef VKL_FRAMEDATA_HPP
#define VKL_FRAMEDATA_HPP
#include <stdint.h>
#include <array>

#include <vulkan/vulkan.h>

#include <common/delstack.hpp>
#include <vulkan_low/desc_allocgrow.hpp>

namespace vkl_core {
class VKCore;
}

namespace vkl_frames {
/**
* @brief Nubmer of frames used in rendering pipeline.
* @details Setting this variable to two acts as a double-buffer for the
* commands, allowing us to draw while a command buffer is being executed.
*/
constexpr uint8_t FRAME_OVERLAP = 2;

/**
 * @struct Frame
 * @brief An abstraction containing all of the data to produce a frame in the swapchain.
 * @author Spencer Banasik
 */
struct Frame {

    /**
     * @brief A synchronization variable for GPU -> CPU communication.
     * @details The fence is used by the GPU to tell the CPU that it is finished
     * with the command buffer. We wait for the fence before using the resources associated
     * with it. You'll see this used at the beginning of the render loop.
     */
    VkFence render_fence{};

    /**
     * @brief Synchronization variables for GPU -> GPU syncrhonization.
     * @details The swampchain_semaphore and render_semaphore are used to
     * organize the order that GPU commands are run. In our case,
     * swapchain_semaphore is used to have the render commands wait for the
     * swapchain, and the render_semaphore will control when the image
     * is presented for the OS. These are set at the end of the render loop.
     */
    VkSemaphore swapchain_semaphore{}, render_semaphore{};

    /**
     * @brief Allocator for a command buffer.
     * @details A pool can have multiple buffers allocated to it,
     * but we can only record commands from one thread at a time,
     * so we'll pair the allocators and buffers together.
     * @note One use for having multiple buffers with a pool is if you
     * wanted to store and reuse buffers.
     */
    VkCommandPool command_pool{};

    /**
     * @brief Command buffer to hold draw commands.
     * @details Command buffers begin recording by calling `vkBeginCommandBuffer()`,
     * allowing it to receive drawing commands. Once done, call `vkEndCommandBuffer()`
     * and it is ready for execution.
     * `vkQueueSubmit()` submits the buffer to a queue and is pending. Once the GPU is done
     * `vkResetCommandBuffer()` can be called.
     */
    VkCommandBuffer command_buffer{};

    /**
     * @brief Holds functions to execute.
     * @details Descriptors accumulate pipelines, shaders, and other items that have
     * manual deletion functions. Unless/until these get RAII, this is used to handle their
     * lifetimes and resource cleanup.
     */
    com::DeletionStack deletion_stack;

    /**
     * @brief An allocator for descriptors
     * @todo implement documentation
     */
    vkl_desc::DescriptorAllocatorGrowable frame_descriptors;
};

/**
* @class FrameData
* @brief A simple RAII wrapper for the Frame abstraction.
* @author Spencer Banasik
*/
class FrameData {
public:

    FrameData(vkl_core::VKCore* core);
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

    /**
     * @brief Frames hold draw and synchronization structures
     */
    std::array<Frame, FRAME_OVERLAP> active_frames;
private:
    vkl_core::VKCore* core_handle;
};
}
#endif
