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
			uint64_t occupancy;
			uint64_t capacity;
			util::multIndex<uint64_t, uint64_t> sReg;
		public:
			trackedMemory();
			void init(vk::Device, vk::DeviceMemory src, uint64_t size);
			void* tryAlloc(uint64_t bytes, uint64_t alignment);
			core::vPointer alloc(uint64_t bytes, uint64_t align, void* key);
			core::vPointer malloc(uint64_t bytes, uint64_t alignment);
			virtual void free(core::vPointer ptr);
			virtual ~trackedMemory();
		};
		
		class StreamHost;

		struct Vueue {
			vk::Device device;
			core::vPointer mem;
			vk::Buffer buffer;
			uint64_t size;
			Vueue();
			Vueue(vk::Device device, core::vPointer mem, vk::Buffer buffer, uint64_t size);
			void bindBuffer();
			void cleanUp();
		};

		struct StageVueue : public Vueue {
			StageVueue();
			StageVueue(vk::Device device, core::vPointer mem, vk::Buffer buffer, uint64_t size);
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
			uint64_t size;
			vk::BufferCopy cpy;
		public:
			StreamHandle();
			StreamHandle(StreamHost& src, vk::CommandBuffer cmd, uint64_t size, Vueue stage, Vueue vram);
			void initialize(StreamHost& src, vk::CommandBuffer cmd, uint64_t size, Vueue stage, Vueue vram);
			vk::Fence transfer();
			bool transferComplete();
			Vueue collectVram();
		};

		class StreamHost{
			uint64_t rId;
			uint64_t stageBlockSize;
			uint64_t vramBlockSize;
			uint64_t atomSize;
			std::mutex sync;
			vk::PhysicalDevice pDevice;
			vk::Device device;
			uint32_t queueIndex;
			vk::CommandPool cmd;
			std::vector<vk::Queue>* transferQueue;
			core::vMemory stage;
			std::map<uint32_t, std::vector<trackedMemory>> vram;
			void init();
		public:
			StreamHost();
			StreamHost(vk::PhysicalDevice pd, vk::Device device, uint32_t queueIndex, std::vector<vk::Queue>& transferQueue, uint64_t vramBlockSize, uint64_t stageBlockSize);
			void initialize(vk::PhysicalDevice pd, vk::Device device, uint32_t queueIndex, std::vector<vk::Queue>& transferQueue, uint64_t vramBlockSize, uint64_t stageBlockSize);
			vk::Device getDevice();
			StageVueue allocateStageBuffer(uint64_t size);
			Vueue allocateVRAM(vk::Buffer dst, uint64_t size);
			StreamHandle allocateStream(Vueue src, Vueue dst);
			vk::Queue& requestQueue();
			~StreamHost();
		};
	}
}




#endif