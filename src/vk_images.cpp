// Spencer Banasik
// Created: 12/17/2024
// Last Modified: 1/10/2025
#include <vk_images.hpp>

#include <optional>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vk_initializers.hpp>
#include <vk_types.hpp>
#include <vk_core.hpp>
#include <vk_frames.hpp>

gf::AllocatedImage gf::vk_img::ImageBufferAllocator::create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) {
	AllocatedImage new_image;
	new_image.image_format = format;
	new_image.image_size = size;
	VkImageCreateInfo img_info = vk_init::image_info(format, size, usage);
	if (mipmapped)
		img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;

	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vmaCreateImage(alloc_handle->allocator, &img_info, &alloc_info, &new_image.image, &new_image.allocation, nullptr);

	VkImageAspectFlags aspect_flag = VK_IMAGE_ASPECT_COLOR_BIT;
	if (format == VK_FORMAT_D32_SFLOAT)
		aspect_flag = VK_IMAGE_ASPECT_DEPTH_BIT;

	VkImageViewCreateInfo view_info = vk_init::image_view_info(format, new_image.image, aspect_flag);
	view_info.subresourceRange.levelCount = img_info.mipLevels;
	vkCreateImageView(core_handle->device, &view_info, nullptr, &new_image.image_view);

	return new_image;
}
gf::AllocatedImage gf::vk_img::ImageBufferAllocator::create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) {
	size_t data_size = size.depth * size.width * size.height * 4;
	AllocatedBuffer upload_buffer = create_buffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	memcpy(upload_buffer.info.pMappedData, data, data_size);

	AllocatedImage new_image = create_image(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);

	imm_handle->immediate_submit([new_image, upload_buffer, &size, mipmapped](VkCommandBuffer cmd) {
		vk_img::transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkBufferImageCopy copy_region = {};
		copy_region.bufferOffset = 0;
		copy_region.bufferRowLength = 0;
		copy_region.bufferImageHeight = 0;
		copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copy_region.imageSubresource.mipLevel = 0;
		copy_region.imageSubresource.baseArrayLayer = 0;
		copy_region.imageSubresource.layerCount = 1;
		copy_region.imageExtent = size;

		vkCmdCopyBufferToImage(cmd, upload_buffer.buffer, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

		if (mipmapped) {
			generate_mipmaps(cmd, new_image.image, VkExtent2D{ new_image.image_size.width,new_image.image_size.height });
		}
		else {
			transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	});

	destroy_buffer(upload_buffer);

	return new_image;
}
void gf::vk_img::generate_mipmaps(VkCommandBuffer cmd, VkImage image, VkExtent2D imageSize) {
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
		imageBarrier.subresourceRange = vk_init::image_subresource_range(aspectMask);
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
gf::AllocatedBuffer gf::vk_img::ImageBufferAllocator::create_buffer(size_t allocation_size, VkBufferUsageFlags flags, VmaMemoryUsage memory_usage) {
	VkBufferCreateInfo buffer_info = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buffer_info.pNext = nullptr;
	buffer_info.size = allocation_size;
	buffer_info.usage = flags;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = memory_usage;
	vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocatedBuffer new_buffer;
	vmaCreateBuffer(alloc_handle->allocator, &buffer_info, &vmaallocInfo, &new_buffer.buffer, &new_buffer.allocation, &new_buffer.info);

	return new_buffer;
}

void gf::vk_img::ImageBufferAllocator::destroy_image(const AllocatedImage& image) {
	vkDestroyImageView(core_handle->device, image.image_view, nullptr);
	vmaDestroyImage(alloc_handle->allocator, image.image, image.allocation);
}
void gf::vk_img::ImageBufferAllocator::destroy_buffer(const AllocatedBuffer& buffer) {
	vmaDestroyBuffer(alloc_handle->allocator, buffer.buffer, buffer.allocation);
}

void gf::vk_img::transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout) {
	VkImageMemoryBarrier2 image_barrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2, .pNext = nullptr };

	image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	image_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	image_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	image_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	image_barrier.oldLayout = current_layout;
	image_barrier.newLayout = new_layout;

	VkImageAspectFlags aspect_mask = (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	image_barrier.subresourceRange = vk_init::subresource_range(aspect_mask);
	image_barrier.image = image;

	VkDependencyInfo dep_info{};
	dep_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dep_info.pNext = nullptr;

	dep_info.imageMemoryBarrierCount = 1;
	dep_info.pImageMemoryBarriers = &image_barrier;

	vkCmdPipelineBarrier2(cmd, &dep_info);
}
void gf::vk_img::copy_image_to_image(VkCommandBuffer cmd, VkImage src, VkImage dst, VkExtent2D src_size, VkExtent2D dst_size) {
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