#ifndef MEMORYMAP_HPP
#define MEMORYMAP_HPP

#include <vulkan/vulkan.hpp>
#include "../infr/lvm.hpp"

namespace core {
	class vMemory;
	//incomplete draft -- more to be done with interface & abstraction
	class vPointer {
		vMemory* src;
		uint64_t offset;
		uint64_t size;
	public:
		vPointer();
		vPointer(vMemory& src, uint64_t offset, uint64_t size);
		void initialize(vMemory& src, uint64_t offset, uint64_t size);
		uint64_t getOffset() const;
		uint64_t getSize() const;
		vk::DeviceMemory getDeviceMemory();
		void free();
	};

	class vMemory {
	protected:
		vk::Device device;
		vk::DeviceMemory src;
		infr::lvm::LinearVM allocator;
	public:
		vMemory();
		vMemory(vk::Device device, vk::DeviceMemory src, uint64_t size);
		void init(vk::Device device, vk::DeviceMemory src, uint64_t size);
		vk::DeviceMemory getDeviceMemory();
		static vMemory createMemoryPool(vk::Device device, uint64_t size, uint64_t memoryType);
		static uint64_t selectMemoryType(vk::PhysicalDevice device, vk::MemoryPropertyFlags flag, uint32_t typeFilter = 0xFFFFFFFF);
		virtual vPointer malloc(uint64_t bytes, uint64_t alignment = 4, bool opt = true);
		virtual void free(vPointer& ptr);
		virtual ~vMemory();
	};

	vk::DeviceMemory allocateDeviceMemory(vk::PhysicalDevice device, vk::Device lDevice, uint64_t size, uint64_t typeFilter, vk::MemoryPropertyFlagBits flag);
}


#endif