#include <t_alloc_img.hpp>

#include <vkl_imgbuf_alloc.hpp>

vkl_res::AllocatedImage::AllocatedImage(const AllocatedImage& other)
	: image(other.image),
	image_view(other.image_view),
	allocation(other.allocation),
	image_size(other.image_size),
	image_format(other.image_format),
	counter(other.counter),
	allocator(other.allocator) {
}
vkl_res::AllocatedImage::AllocatedImage(AllocatedImage&& other) noexcept
	: image(std::move(other.image)),
	image_view(std::move(other.image_view)),
	allocation(std::move(other.allocation)),
	image_size(std::move(other.image_size)),
	image_format(std::move(other.image_format)),
	counter(std::move(other.counter)),
	allocator(std::move(other.allocator)) {
	other.image = nullptr;
	other.image_view = nullptr;
	other.allocation = nullptr;
}

vkl_res::AllocatedImage::~AllocatedImage() {
	if (counter.can_delete_resources())
		allocator->destroy_image(*this);
}