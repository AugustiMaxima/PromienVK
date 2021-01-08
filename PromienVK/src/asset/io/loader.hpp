#ifndef LOADING_H
#define LOADING_H
#include <map>
#include <vector>
#include <mutex>
#include "../../utils/multindex.hpp"
#include "../../core/memoryMap.hpp"

namespace asset {
	namespace io {
		
		class trackedMemory : public core::vMemory {
		protected:
			int occupancy;
			int capacity;
			util::multIndex<int, int> sReg;
		public:
			trackedMemory();
			void init(vk::Device, vk::DeviceMemory src, int size);
			void* tryAlloc(int bytes, int alignment);
			core::vPointer alloc(int bytes, void* key);
			core::vPointer malloc(int bytes, int alignment);
			virtual void free(core::vPointer ptr);
			virtual ~trackedMemory();
		};
		
		class StreamHost;

		struct Vueue {
			vk::Device device;
			core::vPointer mem;
			vk::Buffer buffer;
			int size;
			Vueue();
			Vueue(vk::Device device, core::vPointer mem, vk::Buffer buffer, int size);
			void bindBuffer();
			void cleanUp();
		};

		struct StageVueue : public Vueue {
			StageVueue();
			StageVueue(vk::Device device, core::vPointer mem, vk::Buffer buffer, int size);
			void* getStageSource();
			void flushCache();
		};

		class StreamHandle {
			StreamHost* src;
			vk::Device device;
			vk::CommandBuffer cmd;
			vk::Fence fence;
			Vueue stage;
			Vueue vram;
			int size;
			vk::BufferCopy cpy;
		public:
			StreamHandle();
			StreamHandle(StreamHost& src, vk::CommandBuffer cmd, int size, Vueue stage, Vueue vram);
			void initialize(StreamHost& src, vk::CommandBuffer cmd, int size, Vueue stage, Vueue vram);
			vk::Fence transfer();
			bool transferComplete();
			Vueue collectVram();
		};

		class StreamHost{
			int rId;
			int stageBlockSize;
			int vramBlockSize;
			std::mutex sync;
			vk::PhysicalDevice pDevice;
			vk::Device device;
			uint32_t queueIndex;
			vk::CommandPool cmd;
			std::vector<vk::Queue>* transferQueue;
			core::vMemory stage;
			std::map<uint32_t, std::vector<trackedMemory>> vram;
			core::vPointer allocateMemory(vk::Buffer dst);
			void init();
		public:
			StreamHost();
			StreamHost(vk::PhysicalDevice pd, vk::Device device, uint32_t queueIndex, std::vector<vk::Queue>& transferQueue, int vramBlockSize, int stageBlockSize);
			void initialize(vk::PhysicalDevice pd, vk::Device device, uint32_t queueIndex, std::vector<vk::Queue>& transferQueue, int vramBlockSize, int stageBlockSize);
			vk::Device getDevice();
			StageVueue allocateStageBuffer(int size);
			Vueue allocateVRAM(vk::Buffer dst, int size);
			StreamHandle allocateStream(Vueue src, Vueue dst);
			vk::Queue& requestQueue();
			~StreamHost();
		};

	}
}




#endif