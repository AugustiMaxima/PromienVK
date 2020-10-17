#include "streamUnit.hpp"
#include <fstream>

namespace core {
	namespace ast {
		streamUnit::streamUnit(vk::Device device, StreamHost& host, std::string assetPath, vk::BufferUsageFlags usage)
			:device(device), host(host), assetPath(assetPath), usage(usage){}

		void streamUnit::work() {
			using namespace std;
			char* bytes;
			ifstream src{ assetPath, ios::ate | ios::binary };
			int size = src.tellg();
			vk::Buffer dst = device.createBuffer(vk::BufferCreateInfo()
				.setSharingMode(vk::SharingMode::eExclusive)
				.setQueueFamilyIndexCount(0)
				.setSize(size)
				.setUsage(usage));
			StreamHandle handler = host.allocateStream(dst, size);
			bytes = (char*)handler.stagingGround();
			src.read(bytes, size);
			src.close();
			handler.flushCache();
			fence = handler.transfer();
		}

		vk::Fence streamUnit::getFence() {
			if(isComplete())
				return fence;
			return nullptr;
		}

		bool streamUnit::transferComplete() {
			return device.getFenceStatus(fence) == vk::Result::eSuccess;
		}

		streamUnit::~streamUnit(){}

	}
}