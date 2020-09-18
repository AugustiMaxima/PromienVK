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
			for (int i = 0; i < prop.memoryTypeCount; i++) {
				if (typeFilter & (1 << i) && (prop.memoryTypes[i].propertyFlags & flag) == flag)
					return i;
			}
		}

		vPointer vMemory::malloc(int bytes) {
			int offset = allocator.malloc(bytes);
			allocRegistry.put(offset, true);
			return vPointer(src, offset);
		}

		void vMemory::free(vPointer ptr) {
			if (ptr.memory != src)
				throw std::exception("This memory was allocated from a different heap of deviceMemory");
			if (!allocRegistry.get(ptr.offset))
				throw std::exception("No pointer of the correct offset found");
			allocator.free(ptr.offset);
		}

		vk::DeviceMemory allocateDeviceMemory(vk::PhysicalDevice device, vk::Device lDevice, int size, int typeFilter, vk::MemoryPropertyFlagBits flag) {
			vk::MemoryAllocateInfo info = vk::MemoryAllocateInfo()
				.setAllocationSize(size)
				.setMemoryTypeIndex(vMemory::selectMemoryType(device, typeFilter, flag));
			return lDevice.allocateMemory(info);
		}

	}
}