#include <vulkan_low/imgbuf_alloc.hpp>

#include <vulkan_low/core.hpp>
#include <vulkan_low/alloc.hpp>
#include <vulkan_low/imm_frame.hpp>
#include <vulkan_low/initializers.hpp>
#include <vulkan_low/images.hpp>

vkl_res::AllocatedImage vkl_res::ImageBufferAllocator::create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) const {
	AllocatedImage new_image(*this);
	new_image.image_format = format;
	new_image.image_size = size;
	VkImageCreateInfo img_info = gf::vk_init::image_info(format, size, usage);
	if (mipmapped)
		img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;

	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	alloc_info.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vmaCreateImage(alloc_handle->allocator, &img_info, &alloc_info, &new_image.image, &new_image.allocation, nullptr);

	VkImageAspectFlags aspect_flag = VK_IMAGE_ASPECT_COLOR_BIT;
	if (format == VK_FORMAT_D32_SFLOAT)
		aspect_flag = VK_IMAGE_ASPECT_DEPTH_BIT;

	VkImageViewCreateInfo view_info = gf::vk_init::image_view_info(format, new_image.image, aspect_flag);
	view_info.subresourceRange.levelCount = img_info.mipLevels;
	vkCreateImageView(core_handle->device, &view_info, nullptr, &new_image.image_view);

	return new_image;
}
vkl_res::AllocatedImage vkl_res::ImageBufferAllocator::create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) const {
	int nr_channels = 4;
	if (format == VK_FORMAT_R8_UNORM)
		nr_channels = 1;

	size_t data_size = size.depth * size.width * size.height * nr_channels;
	AllocatedBuffer upload_buffer = create_buffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	memcpy(upload_buffer.info.pMappedData, data, data_size);

	AllocatedImage new_image = create_image(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);

	imm_handle->immediate_submit([new_image, upload_buffer, &size, mipmapped](VkCommandBuffer cmd) {
		vkl_img::transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

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
			vkl_img::generate_mipmaps(cmd, new_image.image, VkExtent2D{ new_image.image_size.width,new_image.image_size.height });
		}
		else {
			vkl_img::transition_image(cmd, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
		});

	return new_image;
}
vkl_res::AllocatedBuffer vkl_res::ImageBufferAllocator::create_buffer(size_t allocation_size, VkBufferUsageFlags flags, VmaMemoryUsage memory_usage) const {
	VkBufferCreateInfo buffer_info = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buffer_info.pNext = nullptr;
	buffer_info.size = allocation_size;
	buffer_info.usage = flags;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = memory_usage;
	vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocatedBuffer new_buffer(*this);
	vmaCreateBuffer(alloc_handle->allocator, &buffer_info, &vmaallocInfo, &new_buffer.buffer, &new_buffer.allocation, &new_buffer.info);

	return new_buffer;
}
void vkl_res::ImageBufferAllocator::destroy_image(const AllocatedImage& image) const {
	vkDestroyImageView(core_handle->device, image.image_view, nullptr);
	vmaDestroyImage(alloc_handle->allocator, image.image, image.allocation);
}
void vkl_res::ImageBufferAllocator::destroy_buffer(const AllocatedBuffer& buffer) const {
	vmaDestroyBuffer(alloc_handle->allocator, buffer.buffer, buffer.allocation);
}