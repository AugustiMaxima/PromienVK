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
			cpy.setSize(size).setSrcOffset(stage.getOffset()).setDstOffset(vram.getOffset());
			cmd.begin(vk::CommandBufferBeginInfo());
			cmd.copyBuffer(ss, vs, cpy);
			cmd.end();
			vk::Queue& q = src.requestQueue();
			q.submit(vk::SubmitInfo()
				.setCommandBuffers(cmd), fence);
			return fence;
		}

		bool StreamHandle::transferComplete() {
			if (device.getFenceStatus(fence) == vk::Result::eSuccess)
				return true;
			else
				return false;
		}

		void StreamHandle::purgeStage() {
			device.destroy(ss);
			stage.free();
		}

		Vueue StreamHandle::collectVram() {
			return Vueue{ device, vram, vs };
		}

		vramProxy::vramProxy(ram::vMemory src, int capacity) :src(src), capacity(capacity), occupancy(0) {}

		bool vramProxy::haveCapacity(int size) {
			if (occupancy + size < capacity * 4 / 5) //the 80% utilization is a magic number, need more work here
				return true;
			else
				return false;
		}

		ram::vPointer vramProxy::allocate(int size) {
			occupancy += size;
			ram::vPointer vp = src.malloc(size);
			registry[vp.getOffset()] = size;
			return vp;
		}

		void vramProxy::free(ram::vPointer vp) {
			int size = registry[vp.getOffset()];
			registry.erase(vp.getOffset());
			occupancy -= size;
		}	

		StreamHost::StreamHost(vk::PhysicalDevice pd, vk::Device device, int queueIndex, std::vector<vk::Queue>& transferQueue, int granularity, int stage)
			:rId(0), pDevice(pd), device(device), queueIndex(queueIndex), granularity(granularity), stage(stage), transferQueue(transferQueue){
			using vm = ram::vMemory;
			cmd = device.createCommandPool(vk::CommandPoolCreateInfo()
				.setQueueFamilyIndex(queueIndex));
			vk::DeviceMemory mstage = device.allocateMemory(vk::MemoryAllocateInfo()
				.setAllocationSize(stage)
				.setMemoryTypeIndex(vm::selectMemoryType(pd, vk::MemoryPropertyFlagBits::eHostVisible)));
			this->stage.init(device, mstage);
		}

		vk::Device StreamHost::getDevice() {
			return device;
		}

		StreamHandle StreamHost::allocateStream(vk::Buffer dst, int size) {
			vk::CommandBuffer cmdb = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo()
				.setCommandBufferCount(1)
				.setCommandPool(cmd)
				.setLevel(vk::CommandBufferLevel::ePrimary))[0];
			vk::Buffer stgr = device.createBuffer(vk::BufferCreateInfo()
				.setPQueueFamilyIndices(&queueIndex)
				.setQueueFamilyIndexCount(1)
				.setSharingMode(vk::SharingMode::eExclusive)
				.setSize(size)
				.setUsage(vk::BufferUsageFlagBits::eTransferSrc));



		}
	}
