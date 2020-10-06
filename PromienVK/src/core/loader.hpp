#ifndef LOADING_H
#define LOADING_H
#include <map>
#include <vector>
#include <mutex>
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

		class vramProxy {
			ram::vMemory src;
			int capacity;
			int occupancy;
			std::map<int, int> registry;
		public:
			vramProxy(ram::vMemory src, int capacity);
			bool haveCapacity(int size);
			ram::vPointer allocate(int size);
			void free(ram::vPointer vp);
		};

		class StreamHost{
			int rId;
			int granularity;
			std::mutex sync;
			vk::PhysicalDevice pDevice;
			vk::Device device;
			uint32_t queueIndex;
			vk::CommandPool cmd;
			std::vector<vk::Queue>& transferQueue;
			ram::vMemory stage;
			std::map<uint32_t, std::vector<ram::vMemory>> vram;
		public:
			StreamHost(vk::PhysicalDevice pd, vk::Device, int queueIndex, std::vector<vk::Queue>& transferQueue, int granularity, int stage);
			vk::Device getDevice();
			StreamHandle allocateStream(vk::Buffer dst, int size);
			vk::Queue& requestQueue();
			~StreamHost();
		};

	}
}




#endif