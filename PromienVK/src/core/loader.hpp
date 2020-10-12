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
			ram::vPointer allocateMemory(uint32_t type, int bytes, int alignment);
		public:
			StreamHost(vk::PhysicalDevice pd, vk::Device, uint32_t queueIndex, std::vector<vk::Queue>& transferQueue, int granularity, int stage);
			vk::Device getDevice();
			StreamHandle allocateStream(vk::Buffer dst, int size);
			vk::Queue& requestQueue();
			~StreamHost();
		};

	}
}




#endif