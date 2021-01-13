#include "memoryMap.hpp"

namespace core {
	vPointer::vPointer(){}

	vPointer::vPointer(vMemory& src, uint64_t offset, uint64_t size) : src(&src), offset(offset), size(size){}

	void vPointer::initialize(vMemory& src, uint64_t offset, uint64_t size) {
		this->src = &src;
		this->offset = offset;
		this->size = size;
	}

	uint64_t vPointer::getOffset() const {
		return offset;
	}

	uint64_t vPointer::getSize() const {
		return size;
	}

	vk::DeviceMemory vPointer::getDeviceMemory() {
		return src->getDeviceMemory();
	}

	void vPointer::free() {
		src->free(*this);
	}

	vMemory::vMemory(){}

	vMemory::vMemory(vk::Device device, vk::DeviceMemory src, uint64_t size) : device(device), src(src), allocator(size){}

	vk::DeviceMemory vMemory::getDeviceMemory() {
		return src;
	}

	vMemory vMemory::createMemoryPool(vk::Device device, uint64_t size, uint64_t memoryType) {
		vk::DeviceMemory vram = device.allocateMemory(vk::MemoryAllocateInfo()
			.setAllocationSize(size)
			.setMemoryTypeIndex(memoryType));
		return vMemory(device, vram, size);
	}

	void vMemory::init(vk::Device device, vk::DeviceMemory src, uint64_t size) {
		this->device = device;
		this->src = src;
		allocator.initialize(size);
	}

	uint32_t vMemory::selectMemoryType(vk::PhysicalDevice device, vk::MemoryPropertyFlags flag, uint32_t typeFilter) {
		vk::PhysicalDeviceMemoryProperties prop = device.getMemoryProperties();
		for (uint64_t i = 0; i < prop.memoryTypeCount; i++) {
			if (typeFilter & (1 << i) && (prop.memoryTypes[i].propertyFlags & flag) == flag)
				return i;
		}
		return -1;
	}

	vPointer vMemory::malloc(uint64_t bytes, uint64_t alignment, bool opt) {
		bytes = allocator.upscale(bytes, alignment);
		uint64_t offset = allocator.malloc(bytes, alignment, opt);
		return vPointer(*this, offset, bytes);
	}

	vMemory::~vMemory(){}

	void vMemory::free(vPointer& ptr) {
		if (ptr.getDeviceMemory() != src)
			throw std::exception("This memory was allocated from a different heap of deviceMemory");
		allocator.free(ptr.getOffset());
	}

	vk::DeviceMemory allocateDeviceMemory(vk::PhysicalDevice device, vk::Device lDevice, uint64_t size, uint64_t typeFilter, vk::MemoryPropertyFlagBits flag) {
		vk::MemoryAllocateInfo info = vk::MemoryAllocateInfo()
			.setAllocationSize(size)
			.setMemoryTypeIndex(vMemory::selectMemoryType(device, flag, typeFilter));
		return lDevice.allocateMemory(info);
	}
}