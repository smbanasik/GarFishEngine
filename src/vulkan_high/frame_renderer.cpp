#include <vulkan_high/frame_renderer.hpp>

#include <vulkan_low/descriptors/desc_layoutbuilder.hpp>
#include <vulkan_low/descriptors/desc_writer.hpp>
#include <vulkan_low/initializers.hpp>

gs::FrameRenderer::FrameRenderer(GraphicsContext& graphics_context, gf::wi::Extent2D dims)
    : device_handle(graphics_context.core.device) {
    VkExtent3D image_size = {dims.width, dims.height, 1};
    draw_image.image_format = VK_FORMAT_R16G16B16A16_SFLOAT;
    draw_image.image_size = image_size;
    VkImageUsageFlags draw_image_usage{};
    draw_image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    draw_image_usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    draw_image_usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    draw_image_usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VmaAllocationCreateInfo image_alloc_info = {};
    image_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    image_alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkImageCreateInfo image_info = gf::vk_init::image_info(draw_image.image_format, image_size, draw_image_usage);
    vmaCreateImage(graphics_context.alloc.allocator, &image_info, &image_alloc_info, &draw_image.image,
                   &draw_image.allocation, nullptr);
    VkImageViewCreateInfo view_info
        = gf::vk_init::image_view_info(draw_image.image_format, draw_image.image, VK_IMAGE_ASPECT_COLOR_BIT);
    vkCreateImageView(graphics_context.core.device, &view_info, nullptr, &draw_image.image_view);

    depth_image.image_format = VK_FORMAT_D32_SFLOAT;
    depth_image.image_size = image_size;
    VkImageUsageFlags depth_usage_flags{};
    depth_usage_flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VkImageCreateInfo depth_image_info
        = gf::vk_init::image_info(depth_image.image_format, image_size, depth_usage_flags);
    vmaCreateImage(graphics_context.alloc.allocator, &depth_image_info, &image_alloc_info, &depth_image.image,
                   &depth_image.allocation, nullptr);
    VkImageViewCreateInfo depth_view_info
        = gf::vk_init::image_view_info(depth_image.image_format, depth_image.image, VK_IMAGE_ASPECT_DEPTH_BIT);
    vkCreateImageView(graphics_context.core.device, &depth_view_info, nullptr, &depth_image.image_view);

    vkl_desc::DescriptorLayoutBuilder builder;
    builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    draw_image_descriptor_layout = builder.build(graphics_context.core.device, VK_SHADER_STAGE_COMPUTE_BIT);
    draw_image_descriptors = graphics_context.global_descriptor_allocator.allocate(graphics_context.core.device,
                                                                                   draw_image_descriptor_layout);

    vkl_desc::DescriptorWriter writer;
    writer.write_image(0, draw_image.image_view, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL,
                       VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    writer.update_set(graphics_context.core.device, draw_image_descriptors);
}

gs::FrameRenderer::~FrameRenderer() {
    vkDestroyDescriptorSetLayout(device_handle, draw_image_descriptor_layout, nullptr);
}

bool gs::FrameRenderer::is_visible(const vkl::RenderObject& obj, const glm::mat4 viewproj) {
    std::array<glm::vec3, 8> corners{
        glm::vec3{1, 1, 1},  glm::vec3{1, 1, -1},  glm::vec3{1, -1, 1},  glm::vec3{1, -1, -1},
        glm::vec3{-1, 1, 1}, glm::vec3{-1, 1, -1}, glm::vec3{-1, -1, 1}, glm::vec3{-1, -1, -1},
    };

    glm::mat4 matrix = viewproj * obj.transform;

    glm::vec3 min = {1.5, 1.5, 1.5};
    glm::vec3 max = {-1.5, -1.5, -1.5};

    for (int c = 0; c < 8; c++) {
        // project each corner into clip space
        glm::vec4 v = matrix * glm::vec4(obj.bounds.origin + (corners[c] * obj.bounds.extents), 1.f);

        // perspective correction
        v.x = v.x / v.w;
        v.y = v.y / v.w;
        v.z = v.z / v.w;

        min = glm::min(glm::vec3{v.x, v.y, v.z}, min);
        max = glm::max(glm::vec3{v.x, v.y, v.z}, max);
    }

    // check the clip space box is within the view
    if (min.z > 1.f || max.z < 0.f || min.x > 1.f || max.x < -1.f || min.y > 1.f || max.y < -1.f) {
        return false;
    } else {
        return true;
    }
}