#include "loader.hpp"

namespace core {
	namespace ast {
		using namespace ram;
		LoadPoint::LoadPoint(vk::Device device, int size, vPointer vram, vPointer stage):device(device), size(size), vram(vram), stage(stage){}

		void* LoadPoint::stagingGround() {
			return device.mapMemory(stage.memory, stage.offset, size);
		}

		void LoadPoint::flushCache() {
			vk::MappedMemoryRange rng = vk::MappedMemoryRange()
				.setMemory(stage.memory)
				.setOffset(stage.offset)
				.setSize(size);
			device.flushMappedMemoryRanges(rng);
		}

	}
}