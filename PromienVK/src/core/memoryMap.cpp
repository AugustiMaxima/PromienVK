#include "memoryMap.hpp"

namespace core {
	namespace ram {
		vPointer::vPointer(vMemory& src, int offset) : src(src), offset(offset){}

		int vPointer::getOffset() const {
			return offset;
		}

		vk::DeviceMemory vPointer::getDeviceMemory() {
			return src.getDeviceMemory();
		}

		void vPointer::free() {
			src.free(*this);
		}

		vMemory::vMemory(vk::Device device, vk::DeviceMemory src, int size) : device(device), src(src), allocator(size){}

		vk::DeviceMemory vMemory::getDeviceMemory() {
			return src;
		}

		vMemory::vMemory(int size):allocator(size) {}

		vMemory vMemory::createMemoryPool(vk::Device device, int size, int memoryType) {
			vk::DeviceMemory vram = device.allocateMemory(vk::MemoryAllocateInfo()
				.setAllocationSize(size)
				.setMemoryTypeIndex(memoryType));
			return vMemory(device, vram, size);
		}

		void vMemory::init(vk::Device device, vk::DeviceMemory src) {
			device = device;
			src = src;
		}

		int vMemory::selectMemoryType(vk::PhysicalDevice device, vk::MemoryPropertyFlags flag, uint32_t typeFilter) {
			vk::PhysicalDeviceMemoryProperties prop = device.getMemoryProperties();
			for (int i = 0; i < prop.memoryTypeCount; i++) {
				if (typeFilter & (1 << i) && (prop.memoryTypes[i].propertyFlags & flag) == flag)
					return i;
			}
		}

		vPointer vMemory::malloc(int bytes, int alignment) {
			int offset = allocator.malloc(bytes, alignment);
			allocRegistry.put(offset, true);
			return vPointer(*this, offset);
		}

		vMemory::~vMemory(){}

		void vMemory::free(vPointer ptr) {
			if (ptr.getDeviceMemory() != src)
				throw std::exception("This memory was allocated from a different heap of deviceMemory");
			if (!allocRegistry.get(ptr.getOffset()))
				throw std::exception("No pointer of the correct offset found");
			allocator.free(ptr.getOffset());
		}

		vk::DeviceMemory allocateDeviceMemory(vk::PhysicalDevice device, vk::Device lDevice, int size, int typeFilter, vk::MemoryPropertyFlagBits flag) {
			vk::MemoryAllocateInfo info = vk::MemoryAllocateInfo()
				.setAllocationSize(size)
				.setMemoryTypeIndex(vMemory::selectMemoryType(device, flag, typeFilter));
			return lDevice.allocateMemory(info);
		}

	}
}