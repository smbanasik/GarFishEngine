#include <vkl_imm_frame.hpp>

#include <vkl_core.hpp>
#include <vkl_initializers.hpp>

VkFence imm_fence;
VkCommandBuffer imm_command_buffer;
VkCommandPool imm_command_pool;

vkl_frames::ImmediateFrame::ImmediateFrame(vkl_core::VKCore* core)
    : core_handle(core) {
    VkCommandPoolCreateInfo pool_info = gf::vk_init::command_pool_info(core_handle->graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    vkCreateCommandPool(core_handle->device, &pool_info, nullptr, &imm_command_pool);
    VkCommandBufferAllocateInfo cmd_alloc_info = gf::vk_init::command_allocate_info(imm_command_pool);
    vkAllocateCommandBuffers(core_handle->device, &cmd_alloc_info, &imm_command_buffer);
    VkFenceCreateInfo fence_info = gf::vk_init::fence_info(VK_FENCE_CREATE_SIGNALED_BIT);
    vkCreateFence(core_handle->device, &fence_info, nullptr, &imm_fence);
}
vkl_frames::ImmediateFrame::~ImmediateFrame() {
    if (imm_command_pool != nullptr) {
        vkDestroyCommandPool(core_handle->device, imm_command_pool, nullptr);
        vkDestroyFence(core_handle->device, imm_fence, nullptr);
    }
}
vkl_frames::ImmediateFrame::ImmediateFrame(ImmediateFrame&& other) noexcept
    : imm_fence(std::move(other.imm_fence)), imm_command_buffer(std::move(other.imm_command_buffer)),
    imm_command_pool(std::move(other.imm_command_pool)), core_handle(std::move(other.core_handle)) {
    other.imm_command_pool = nullptr;
    other.imm_fence = nullptr;
}
void vkl_frames::ImmediateFrame::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function) {
    vkResetFences(core_handle->device, 1, &imm_fence);
    vkResetCommandBuffer(imm_command_buffer, 0);
    VkCommandBuffer cmd = imm_command_buffer;

    VkCommandBufferBeginInfo cmd_begin_info = gf::vk_init::begin_command(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkBeginCommandBuffer(cmd, &cmd_begin_info);
    function(cmd);
    vkEndCommandBuffer(cmd);

    VkCommandBufferSubmitInfo cmd_submit_info = gf::vk_init::submit_command(cmd);
    VkSubmitInfo2 submit = gf::vk_init::submit_info(&cmd_submit_info, nullptr, nullptr);
    vkQueueSubmit2(core_handle->graphics_queue, 1, &submit, imm_fence);
    vkWaitForFences(core_handle->device, 1, &imm_fence, true, 1000000000);
}