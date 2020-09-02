#ifndef MEMORYMAP_HPP
#define MEMORYMAP_HPP

#include <vulkan/vulkan.hpp>
#include "../infr/lvm.hpp"

namespace core {
	namespace ram {
		//incomplete draft -- more to be done with interface & abstraction
		struct vPointer {
			const vk::DeviceMemory memory;
			const int offset;
			vPointer(vk::DeviceMemory memory, int offset);
		};

		class vMemory {
			vk::DeviceMemory src;
			infr::lvm::LinearVM allocator;
			util::multIndex<int, bool> allocRegistry; //An additional safety check to prevent bad free
		public:
			vMemory(vk::DeviceMemory src, int size);
			static vMemory createMemoryPool(vk::Device device, int size, int memoryType);
			static int selectMemoryType(vk::PhysicalDevice device, int typeFilter, vk::MemoryPropertyFlags flag);
			vPointer malloc(int bytes);
			void free(vPointer ptr);
		};

		vk::DeviceMemory allocateDeviceMemory(vk::PhysicalDevice device, vk::Device lDevice, int size, int typeFilter, vk::MemoryPropertyFlagBits flag);
	}
}




#endif