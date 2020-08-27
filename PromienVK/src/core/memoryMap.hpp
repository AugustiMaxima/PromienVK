#ifndef MEMORYMAP_HPP
#define MEMORYMAP_HPP

#include <vulkan/vulkan.hpp>
#include "../infr/lvm.hpp"

namespace core {
	namespace ram {
		//incomplete draft -- more to be done with interface & abstraction
		struct vPointer {
			vk::DeviceMemory memory;
			int offset;
			vPointer(vk::DeviceMemory memory, int offset);
		};

		class vMemory {
			vk::DeviceMemory src;
			infr::lvm::LinearVM allocator;
		public:
			vMemory(vk::DeviceMemory src, int size);
			static vMemory createMemoryPool(vk::Device device, int size, int memoryType);
			static int selectMemoryType(vk::PhysicalDevice device, int typeFilter, vk::MemoryPropertyFlags flag);
		};


	}
}




#endif