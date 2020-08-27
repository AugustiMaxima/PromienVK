#include "memoryMap.hpp"

namespace core {
	namespace ram {
		vPointer::vPointer(vk::DeviceMemory memory, int offset):memory(memory), offset(offset){}

		vMemory::vMemory(vk::DeviceMemory src, int size) : src(src), allocator{ size, 4 }{}

		vMemory vMemory::createMemoryPool(vk::Device device, int size, int memoryType) {
			vk::DeviceMemory vram = device.allocateMemory(vk::MemoryAllocateInfo()
				.setAllocationSize(size)
				.setMemoryTypeIndex(memoryType));
			return vMemory(vram, size);
		}

		int vMemory::selectMemoryType(vk::PhysicalDevice device, int typeFilter, vk::MemoryPropertyFlags flag) {
			vk::PhysicalDeviceMemoryProperties prop = device.getMemoryProperties();
			
		}
	}
}