#ifndef LOADING_H
#define LOADING_H
#include <map>
#include <vector>
#include "memoryMap.hpp"

namespace core {
	namespace ast {
		class StreamHost;

		struct Vueue {
			vk::Device device;
			ram::vPointer mem;
			vk::Buffer buffer;
			void bindBuffer();
			void cleanUp();
		};

		class StreamHandle {
			StreamHost& src;
			vk::Device device;
			vk::CommandBuffer cmd;
			vk::Fence fence;
			int size;
			ram::vPointer vram;
			ram::vPointer stage;
			vk::Buffer vs;
			vk::Buffer ss;
			vk::BufferCopy cpy;
		public:
			StreamHandle(StreamHost& src, vk::CommandBuffer buffer, int size, ram::vPointer vram, ram::vPointer stage, vk::Buffer vs, vk::Buffer ss);
			void* stagingGround();
			void flushCache();
			vk::Fence transfer();
			bool transferComplete();
			void purgeStage();
			Vueue collectVram();
		};

		class StreamHost{
			vk::Device device;
			vk::CommandPool cmd;
			ram::vMemory vram;
			ram::vMemory stage;
		public:
			StreamHost(vk::Device, int vram, int stage);
			vk::Device getDevice();
			StreamHandle allocateStream(vk::Buffer dst, int size);
			~StreamHost();
		};

	}
}




#endif