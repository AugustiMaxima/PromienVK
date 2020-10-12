#include "loader.hpp"

namespace core {
	namespace ast {
		trackedMemory::trackedMemory(int size) :ram::vMemory(size), capacity(size), occupancy(0) {}

		void trackedMemory::init(vk::Device device, vk::DeviceMemory src) {
			ram::vMemory::init(device, src);
		}

		void* trackedMemory::tryAlloc(int bytes, int alignment) {
			//80% is a megic number
			//we may switch to attempts instead, but so far this magic number have worked well
			if (occupancy + bytes < capacity * 4 / 5) {
				return allocator.try_alloc(bytes, alignment);
			}
			return nullptr;
		}

		ram::vPointer trackedMemory::alloc(int bytes, void* key) {
			occupancy += bytes;
			int offset = allocator.fin_alloc((infr::lvm::rNode*)key, bytes);
			sReg.put(offset, bytes);
			return ram::vPointer(*this, offset);
		}
		//this occupancy is coarse grained and misses padding info
		ram::vPointer trackedMemory::malloc(int bytes, int alignment) {
			occupancy += bytes;
			int offset = allocator.malloc(bytes, alignment);
			sReg.put(offset, bytes);
			return ram::vPointer(*this, offset);
		}

		void trackedMemory::free(ram::vPointer ptr) {
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

		StreamHandle::StreamHandle(StreamHost& src, vk::CommandBuffer cmd, int size, ram::vPointer vram, ram::vPointer stage, vk::Buffer vs, vk::Buffer ss)
			:src(src), cmd(cmd), size(size), vram(vram), stage(stage), vs(vs), ss(ss) {
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

		ram::vPointer StreamHost::allocateMemory(uint32_t type, int bytes, int alignment) {
			std::vector<trackedMemory>& vrs = vram[type];
			for (auto& vm : vrs) {
				void* k = vm.tryAlloc(bytes, alignment);
				if (k) {
					return vm.alloc(bytes, k);
				}
			}
			vk::DeviceMemory dm = device.allocateMemory(vk::MemoryAllocateInfo()
				.setAllocationSize(granularity)
				.setMemoryTypeIndex(type));
			vrs.emplace_back(granularity);
			trackedMemory& vm = vrs[vrs.size() - 1];
			vm.init(device, dm);
			return vm.malloc(bytes, alignment);
		}

		StreamHost::StreamHost(vk::PhysicalDevice pd, vk::Device device, uint32_t queueIndex, std::vector<vk::Queue>& transferQueue, int granularity, int stage)
			:rId(0), pDevice(pd), device(device), queueIndex(queueIndex), granularity(granularity), stage(stage), transferQueue(transferQueue) {
			using vm = ram::vMemory;
			cmd = device.createCommandPool(vk::CommandPoolCreateInfo()
				.setQueueFamilyIndex(queueIndex));

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
			vk::MemoryRequirements stgprop = device.getBufferMemoryRequirements(dst);
			using vp = ram::vPointer;
			vp stgp = stage.malloc(stgprop.size, stgprop.alignment);
			vk::MemoryRequirements prop = device.getBufferMemoryRequirements(dst);
			using vm = ram::vMemory;
			uint32_t index = vm::selectMemoryType(pDevice, vk::MemoryPropertyFlagBits::eDeviceLocal, prop.memoryTypeBits);
			vp vrmp = allocateMemory(index, prop.size, prop.alignment);
			return StreamHandle(*this, cmdb, size, vrmp, stgp, dst, stgr);
		}

		vk::Queue& StreamHost::requestQueue() {
			return transferQueue[0];
		}

		StreamHost::~StreamHost() {

		}
	}
}