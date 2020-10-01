#include "loader.hpp"

namespace core {
	namespace ast {
		void Vueue::bindBuffer() {
			device.bindBufferMemory(buffer, mem.getDeviceMemory(), mem.getOffset());
		}

		void Vueue::cleanUp() {
			mem.free();
			device.destroyBuffer(buffer);
		}

		StreamHandle::StreamHandle(StreamHost& src, vk::CommandBuffer cmd, int size, ram::vPointer vram, ram::vPointer stage, vk::Buffer vs, vk::Buffer ss)
		:src(src), cmd(cmd), size(size), vram(vram), stage(stage), vs(vs), ss(ss){
			device = src.getDevice();
		}

		void* StreamHandle::stagingGround() {
			return device.mapMemory(stage.getDeviceMemory(), stage.getOffset(), size);
		}

		void StreamHandle::flushCache() {
			vk::MappedMemoryRange range = vk::MappedMemoryRange()
				.setMemory(stage.getDeviceMemory())
				.setOffset(stage.getOffset())
				.setSize(size);
			device.flushMappedMemoryRanges(range);
		}

		vk::Fence StreamHandle::transfer() {
			fence = device.createFence(vk::FenceCreateInfo());

		}
	}
}