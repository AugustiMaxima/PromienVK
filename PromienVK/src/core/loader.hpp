#ifndef LOADING_H
#define LOADING_H

#include "memoryMap.hpp"

namespace core {
	namespace ast {
		
		class LoadPoint {
			vk::Device device;
			int size;
			ram::vPointer vram;
			ram::vPointer stage;
			LoadPoint(vk::Device device, int size, ram::vPointer vram, ram::vPointer stage);
			void* stagingGround();
			void flushCache();
			vk::Fence transfer();
			void freeStagingBuffer();
			ram::vPointer getVram();
		};

		class Loader{
			vk::Device device;
			ram::vMemory vram;
			ram::vMemory stage;
			Loader(vk::Device device, int vSize, int sSize);
			LoadPoint allocateBuffer(int size);
		};
	}
}




#endif