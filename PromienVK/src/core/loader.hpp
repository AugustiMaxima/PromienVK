#ifndef LOADING_H
#define LOADING_H
#include <map>
#include <vector>
#include <mutex>
#include "../utils/multindex.hpp"
#include "memoryMap.hpp"

namespace core {
	namespace ast {
		
		class trackedMemory : public ram::vMemory {
		protected:
			int occupancy;
			int capacity;
			util::multIndex<int, int> sReg;
		public:
			trackedMemory(int size);
			void init(vk::Device, vk::DeviceMemory src);
			void* tryAlloc(int bytes, int alignment);
			ram::vPointer alloc(int bytes, void* key);
			ram::vPointer malloc(int bytes, int alignment);
			virtual void free(ram::vPointer ptr);
			virtual ~trackedMemory();
		};
		
		class StreamHost;

		struct Vueue {
			vk::Device device;
			ram::vPointer mem;
			vk::Buffer buffer;
			int size;
			void bindBuffer();
			void cleanUp();
		};

		class StreamHandle {
			StreamHost& src;
			vk::Device device;
			vk::CommandBuffer cmd;
			vk::Fence fence;
			Vueue stage;
			Vueue vram;
			int size;
			vk::BufferCopy cpy;
		public:
			StreamHandle(StreamHost& src, vk::CommandBuffer cmd, int size, Vueue stage, Vueue vram);
			void* stagingGround();
			void flushCache();
			vk::Fence transfer();
			bool transferComplete();
			Vueue collectVram();
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
			std::map<uint32_t, std::vector<trackedMemory>> vram;
			ram::vPointer allocateMemory(vk::Buffer dst);
		public:
			StreamHost(vk::PhysicalDevice pd, vk::Device, uint32_t queueIndex, std::vector<vk::Queue>& transferQueue, int granularity, int stage);
			vk::Device getDevice();
			Vueue allocateStageBuffer(int size);
			Vueue allocateVRAM(vk::Buffer dst, int size);
			StreamHandle allocateStream(Vueue src, Vueue dst);
			vk::Queue& requestQueue();
			~StreamHost();
		};

	}
}




#endif