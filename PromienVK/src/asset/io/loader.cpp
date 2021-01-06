#include "loader.hpp"

namespace asset {
	namespace io {
		trackedMemory::trackedMemory(int size) :core::vMemory(size), capacity(size), occupancy(0) {}

		void trackedMemory::init(vk::Device device, vk::DeviceMemory src) {
			core::vMemory::init(device, src);
		}

		void* trackedMemory::tryAlloc(int bytes, int alignment) {
			//80% is a megic number
			//we may switch to attempts instead, but so far this magic number have worked well
			if (occupancy + bytes < capacity * 4 / 5) {
				return allocator.try_alloc(bytes, alignment);
			}
			return nullptr;
		}

		core::vPointer trackedMemory::alloc(int bytes, void* key) {
			occupancy += bytes;
			int offset = allocator.fin_alloc((infr::lvm::rNode*)key, bytes);
			sReg.put(offset, bytes);
			return core::vPointer(*this, offset);
		}
		//this occupancy is coarse grained and misses padding info
		core::vPointer trackedMemory::malloc(int bytes, int alignment) {
			occupancy += bytes;
			int offset = allocator.malloc(bytes, alignment);
			sReg.put(offset, bytes);
			return core::vPointer(*this, offset);
		}

		void trackedMemory::free(core::vPointer ptr) {
			std::optional<int> s = sReg.pop(ptr.getOffset());
			if (!s) {
				return;
			}
			occupancy -= s.value();
			allocator.free(ptr.getOffset());
		}

		trackedMemory::~trackedMemory() {}

		void Vueue::bindBuffer() {
			device.bindBufferMemory(buffer, mem.getDeviceMemory(), mem.getOffset());
		}

		void Vueue::cleanUp() {
			mem.free();
			device.destroyBuffer(buffer);
		}

		Vueue::Vueue(vk::Device device, core::vPointer mem, vk::Buffer buffer, int size):device(device), mem(mem), buffer(buffer), size(size){}

		StageVueue::StageVueue(vk::Device device, core::vPointer mem, vk::Buffer buffer, int size):Vueue(device, mem, buffer, size){}

		void* StageVueue::getStageSource() {
			return device.mapMemory(mem.getDeviceMemory(), mem.getOffset(), size);
		}

		void StageVueue::flushCache() {
			vk::MappedMemoryRange range = vk::MappedMemoryRange()
				.setMemory(mem.getDeviceMemory())
				.setOffset(mem.getOffset())
				.setSize(size);
			device.flushMappedMemoryRanges(range);
		}

		StreamHandle::StreamHandle(StreamHost& src, vk::CommandBuffer cmd, int size, Vueue stage, Vueue vram)
			:src(&src), cmd(cmd), size(size), vram(vram), stage(stage) {
			device = src.getDevice();
		}

		vk::Fence StreamHandle::transfer() {
			fence = device.createFence(vk::FenceCreateInfo());
			cpy.setSize(size).setSrcOffset(stage.mem.getOffset()).setDstOffset(vram.mem.getOffset());
			cmd.begin(vk::CommandBufferBeginInfo());
			cmd.copyBuffer(stage.buffer, vram.buffer, cpy);
			cmd.end();
			vk::Queue& q = src->requestQueue();
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

		Vueue StreamHandle::collectVram() {
			//TODO: add cleanup for commandBuffer
			return vram;
		}

		core::vPointer StreamHost::allocateMemory(vk::Buffer dst) {
			vk::MemoryRequirements prop = device.getBufferMemoryRequirements(dst);
			uint32_t type = core::vMemory::selectMemoryType(pDevice, vk::MemoryPropertyFlagBits::eDeviceLocal, prop.memoryTypeBits);
			std::vector<trackedMemory>& vrs = vram[type];
			for (auto& vm : vrs) {
				void* k = vm.tryAlloc(prop.size, prop.alignment);
				if (k) {
					return vm.alloc(prop.size, k);
				}
			}
			vk::DeviceMemory dm = device.allocateMemory(vk::MemoryAllocateInfo()
				.setAllocationSize(granularity)
				.setMemoryTypeIndex(type));
			vrs.emplace_back(granularity);
			trackedMemory& vm = vrs[vrs.size() - 1];
			vm.init(device, dm);
			return vm.malloc(prop.size, prop.alignment);
		}

		StreamHost::StreamHost(vk::PhysicalDevice pd, vk::Device device, uint32_t queueIndex, std::vector<vk::Queue>& transferQueue, int granularity, int stage)
			:rId(0), pDevice(pd), device(device), queueIndex(queueIndex), granularity(granularity), stage(stage), transferQueue(transferQueue) {
			using vm = core::vMemory;
			cmd = device.createCommandPool(vk::CommandPoolCreateInfo()
				.setQueueFamilyIndex(queueIndex)
				.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer));

			vk::Buffer stgr = device.createBuffer(vk::BufferCreateInfo()
				.setPQueueFamilyIndices(&queueIndex)
				.setQueueFamilyIndexCount(1)
				.setSharingMode(vk::SharingMode::eExclusive)
				.setSize(0)
				.setUsage(vk::BufferUsageFlagBits::eTransferSrc));

			vk::MemoryRequirements prop = device.getBufferMemoryRequirements(stgr);

			vk::DeviceMemory mstage = device.allocateMemory(vk::MemoryAllocateInfo()
				.setAllocationSize(stage)
				.setMemoryTypeIndex(vm::selectMemoryType(pd, vk::MemoryPropertyFlagBits::eHostVisible, prop.memoryTypeBits)));
			this->stage.init(device, mstage);
		}

		vk::Device StreamHost::getDevice() {
			return device;
		}

		StageVueue StreamHost::allocateStageBuffer(int size) {
			auto srcInfo = vk::BufferCreateInfo()
				.setPQueueFamilyIndices(&queueIndex)
				.setQueueFamilyIndexCount(1)
				.setSharingMode(vk::SharingMode::eExclusive)
				.setSize(size)
				.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
			vk::Buffer stgr = device.createBuffer(srcInfo);
			vk::MemoryRequirements stgProp = device.getBufferMemoryRequirements(stgr);
			sync.lock();
			core::vPointer stgp = stage.malloc(stgProp.size, stgProp.alignment);
			sync.unlock();
			return StageVueue(device, stgp, stgr, size);
		}

		Vueue StreamHost::allocateVRAM(vk::Buffer dst, int size) {
			vk::MemoryRequirements prop = device.getBufferMemoryRequirements(dst);
			uint32_t type = core::vMemory::selectMemoryType(pDevice, vk::MemoryPropertyFlagBits::eDeviceLocal, prop.memoryTypeBits);
			std::vector<trackedMemory>& vrs = vram[type];
			sync.lock();
			for (auto& vm : vrs) {
				void* k = vm.tryAlloc(prop.size, prop.alignment);
				if (k) {
					Vueue vs{ device, vm.alloc(prop.size, k), dst, size };
					sync.unlock();
					return vs;
				}
			}
			vk::DeviceMemory dm = device.allocateMemory(vk::MemoryAllocateInfo()
				.setAllocationSize(granularity)
				.setMemoryTypeIndex(type));
			vrs.emplace_back(granularity);
			trackedMemory& vm = vrs[vrs.size() - 1];
			vm.init(device, dm);
			Vueue vs{ device, vm.malloc(prop.size, prop.alignment), dst, size };
			sync.unlock();
			return vs;
		}

		StreamHandle StreamHost::allocateStream(Vueue src, Vueue dst) {
			auto cmdInfo = vk::CommandBufferAllocateInfo()
				.setCommandBufferCount(1)
				.setCommandPool(cmd)
				.setLevel(vk::CommandBufferLevel::ePrimary);
			sync.lock();
			vk::CommandBuffer cmdb = device.allocateCommandBuffers(cmdInfo)[0];
			sync.unlock();
			//question -> should vueue hold sizes
			return StreamHandle(*this, cmdb, src.size, src, dst);
		}

		vk::Queue& StreamHost::requestQueue() {
			//synchronization actually optional if you dont mind uneven queues
			sync.lock();
			int r = rId++;
			sync.unlock();
			return transferQueue[r++%transferQueue.size()];
		}

		StreamHost::~StreamHost() {

		}
	}
}