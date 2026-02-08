#include <vulkan_low/alloc_buf.hpp>

#include <vulkan_low/imgbuf_alloc.hpp>

vkl_res::AllocatedBuffer::AllocatedBuffer(const AllocatedBuffer& other)
	: buffer(other.buffer),
	allocation(other.allocation),
	info(other.info),
	counter(other.counter),
	allocator(other.allocator) {
}
vkl_res::AllocatedBuffer::AllocatedBuffer(AllocatedBuffer&& other) noexcept
	: buffer(std::move(other.buffer)),
	allocation(std::move(other.allocation)),
	info(std::move(other.info)),
	counter(std::move(other.counter)),
	allocator(std::move(other.allocator)) {
	other.buffer = nullptr;
	other.allocation = nullptr;
}

vkl_res::AllocatedBuffer::~AllocatedBuffer() {
	if (counter.can_delete_resources())
		allocator->destroy_buffer(*this);
}