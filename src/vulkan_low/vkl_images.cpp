// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 1/10/2025
#include <vkl_images.hpp>

#include <optional>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <stb_image.h>

#include <vkl_initializers.hpp>
#include <vkl_types.hpp>
#include <vkl_core.hpp>
#include <vkl_alloc.hpp>
#include <vkl_imm_frame.hpp>
#include <vkl_imgbuf_alloc.hpp>

#include <vulkan/vk_enum_string_helper.h>
#include <iostream>

void vkl_img::generate_mipmaps(VkCommandBuffer cmd, VkImage image, VkExtent2D imageSize) {
	int mipLevels = int(std::floor(std::log2(std::max(imageSize.width, imageSize.height)))) + 1;
	for (int mip = 0; mip < mipLevels; mip++) {

		VkExtent2D halfSize = imageSize;
		halfSize.width /= 2;
		halfSize.height /= 2;

		VkImageMemoryBarrier2 imageBarrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2, .pNext = nullptr };

		imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
		imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

		imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBarrier.subresourceRange = gf::vk_init::image_subresource_range(aspectMask);
		imageBarrier.subresourceRange.levelCount = 1;
		imageBarrier.subresourceRange.baseMipLevel = mip;
		imageBarrier.image = image;

		VkDependencyInfo depInfo{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .pNext = nullptr };
		depInfo.imageMemoryBarrierCount = 1;
		depInfo.pImageMemoryBarriers = &imageBarrier;

		vkCmdPipelineBarrier2(cmd, &depInfo);

		if (mip < mipLevels - 1) {
			VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

			blitRegion.srcOffsets[1].x = imageSize.width;
			blitRegion.srcOffsets[1].y = imageSize.height;
			blitRegion.srcOffsets[1].z = 1;

			blitRegion.dstOffsets[1].x = halfSize.width;
			blitRegion.dstOffsets[1].y = halfSize.height;
			blitRegion.dstOffsets[1].z = 1;

			blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blitRegion.srcSubresource.baseArrayLayer = 0;
			blitRegion.srcSubresource.layerCount = 1;
			blitRegion.srcSubresource.mipLevel = mip;

			blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blitRegion.dstSubresource.baseArrayLayer = 0;
			blitRegion.dstSubresource.layerCount = 1;
			blitRegion.dstSubresource.mipLevel = mip + 1;

			VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
			blitInfo.dstImage = image;
			blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			blitInfo.srcImage = image;
			blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			blitInfo.filter = VK_FILTER_LINEAR;
			blitInfo.regionCount = 1;
			blitInfo.pRegions = &blitRegion;

			vkCmdBlitImage2(cmd, &blitInfo);

			imageSize = halfSize;
		}
	}

	// transition all mip levels into the final read_only layout
	transition_image(cmd, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
void vkl_img::transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout) {
	VkImageMemoryBarrier2 image_barrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2, .pNext = nullptr };

	image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	image_barrier.oldLayout = current_layout;
	image_barrier.newLayout = new_layout;

	VkImageAspectFlags aspect_mask = (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	image_barrier.subresourceRange = gf::vk_init::subresource_range(aspect_mask);
	image_barrier.image = image;

	VkDependencyInfo dep_info{};
	dep_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dep_info.pNext = nullptr;

	dep_info.imageMemoryBarrierCount = 1;
	dep_info.pImageMemoryBarriers = &image_barrier;

	vkCmdPipelineBarrier2(cmd, &dep_info);
}
void vkl_img::copy_image_to_image(VkCommandBuffer cmd, VkImage src, VkImage dst, VkExtent2D src_size, VkExtent2D dst_size) {
	VkImageBlit2 blit_region{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

	blit_region.srcOffsets[1].x = src_size.width;
	blit_region.srcOffsets[1].y = src_size.height;
	blit_region.srcOffsets[1].z = 1;

	blit_region.dstOffsets[1].x = dst_size.width;
	blit_region.dstOffsets[1].y = dst_size.height;
	blit_region.dstOffsets[1].z = 1;

	blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blit_region.srcSubresource.baseArrayLayer = 0;
	blit_region.srcSubresource.layerCount = 1;
	blit_region.srcSubresource.mipLevel = 0;

	blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blit_region.dstSubresource.baseArrayLayer = 0;
	blit_region.dstSubresource.layerCount = 1;
	blit_region.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blit_info{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
	blit_info.dstImage = dst;
	blit_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blit_info.srcImage = src;
	blit_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blit_info.filter = VK_FILTER_LINEAR;
	blit_info.regionCount = 1;
	blit_info.pRegions = &blit_region;

	vkCmdBlitImage2(cmd, &blit_info);
}

std::optional<vkl_res::AllocatedImage> vkl_img::load_image_from_path(vkl_res::ImageBufferAllocator* allocator, const std::string& file_path) {
	int width, height, nr_channels;

	unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &nr_channels, 4);
	if (data) {
		VkExtent3D imagesize;
		imagesize.width = width;
		imagesize.height = height;
		imagesize.depth = 1;

		vkl_res::AllocatedImage new_image = allocator->create_image(data, imagesize, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, false);

		stbi_image_free(data);
		return new_image;
	}

	return std::nullopt;
}