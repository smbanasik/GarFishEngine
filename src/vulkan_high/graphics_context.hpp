#ifndef GS_GRAPHICS_CONTEXT_HPP
#define GS_GRAPHICS_CONTEXT_HPP

#include <common/delstack.hpp>
#include <common/paths.hpp>
#include <vulkan_low/descriptors/desc_allocgrow.hpp>
#include <vulkan_low/graphics_context/alloc.hpp>
#include <vulkan_low/graphics_context/core.hpp>
#include <vulkan_low/graphics_context/imgbuf_alloc.hpp>
#include <vulkan_low/imm_frame.hpp>

namespace gs {
struct GraphicsContext {

    GraphicsContext(const com::PathFinder& path_finder, gf::wi::WIManager& wi_manager,
                    gf::wi::WInputContext& wi_context)
        : path_finder(path_finder), core(&wi_manager, &wi_context), alloc(core.instance, core.gpu, core.device),
          imm_frame(&core), gpu_allocator(&core, &alloc, &imm_frame) {

        std::vector<vkl_desc::DescriptorAllocatorGrowable::PoolSizeRatio> sizes
            = {{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}};
        global_descriptor_allocator.init(core.device, 10, sizes);
    };
    GraphicsContext(const GraphicsContext& other) = delete;
    GraphicsContext& operator=(const GraphicsContext& other) = delete;
    GraphicsContext(const GraphicsContext&& other) = delete;
    GraphicsContext& operator=(const GraphicsContext&& other) = delete;

    const com::PathFinder& path_finder;
    com::DeletionStack global_deletion_stack;
    vkl_core::VKCore core;
    vkl_core::VMAAllocWrapper alloc;
    vkl_frames::ImmediateFrame imm_frame;
    vkl_desc::DescriptorAllocatorGrowable global_descriptor_allocator;
    vkl_res::ImageBufferAllocator gpu_allocator;
};
} // namespace gs
#endif