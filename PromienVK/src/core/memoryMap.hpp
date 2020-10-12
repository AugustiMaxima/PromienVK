#ifndef MEMORYMAP_HPP
#define MEMORYMAP_HPP

#include <vulkan/vulkan.hpp>
#include "../infr/lvm.hpp"

namespace core {
	namespace ram {
		//incomplete draft -- more to be done with interface & abstraction
		class vPointer {
			vMemory& src;
			int offset;
		public:
			vPointer(vMemory& src, int offset);
			int getOffset() const;
			vk::DeviceMemory getDeviceMemory();
			void free();
		};

		class vMemory {
		protected:
			vk::Device device;
			vk::DeviceMemory src;
			infr::lvm::LinearVM allocator;
			util::multIndex<int, bool> allocRegistry; //An additional safety check to prevent bad free
		public:
			vMemory(int size);
			vMemory(vk::Device device, vk::DeviceMemory src, int size);
			void init(vk::Device device, vk::DeviceMemory src);
			vk::DeviceMemory getDeviceMemory();
			static vMemory createMemoryPool(vk::Device device, int size, int memoryType);
			static int selectMemoryType(vk::PhysicalDevice device, vk::MemoryPropertyFlags flag, uint32_t typeFilter = 0xFFFFFFFF);
			virtual vPointer malloc(int bytes, int alignment = 4);
			virtual void free(vPointer ptr);
			virtual ~vMemory();
		};

		vk::DeviceMemory allocateDeviceMemory(vk::PhysicalDevice device, vk::Device lDevice, int size, int typeFilter, vk::MemoryPropertyFlagBits flag);
	}
}




#endif