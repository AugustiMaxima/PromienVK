#include "deviceLib.hpp"
#include <vector>
#include <map>
#include <functional>
#include <vulkan/vulkan.hpp>
#include "../../infr/type.hpp"
#include "../../utils/heap.hpp"
#include "../Instance.hpp"

namespace lvl {
	std::map<infr::DeviceFunction, std::vector<int>> getSuitableMatches(
		const std::map<infr::DeviceFunction, std::function<int(const lv::PhysicalDevice&)>>& rankMap,
		lv::Instance& instance) {
		std::map<infr::DeviceFunction, std::vector<int>> index;
		const auto& physicalDevices = instance.physicalDevices;
		for (const auto& entry : rankMap) {
			const auto& key = entry.first;
			const auto& rankFunc = entry.second;
			int suitable = 0;
			util::minHeap<int, int> dex;
			for (int i = 0; i < physicalDevices.size(); i++) {
				int rank = rankFunc(instance.physicalDevices[i]);
				if (rank > 0) {
					suitable++;
					dex.insert(rank, i);
				}
			}
			auto& vec = index[key];
			vec.resize(suitable);
			for (int i = suitable - 1; i >= 0; i--) {
				vec[i] = dex.removeMin();
			}
		}
		return index;
	}

	//denominaiton of 1MB
	int sortByVRAM(const lv::PhysicalDevice& device) {
		return device.src.getMemoryProperties().memoryHeaps[0].size / 1048576;
	}

	int vulkanVersionSupport(const lv::PhysicalDevice& device) {
		return device.src.getProperties().apiVersion;
	}

	void autoPopulatePriorities(lv::PhysicalDevice& physicalDevice, std::vector<std::vector<float>>& priorities) {
		const auto queueDscr = physicalDevice.src.getQueueFamilyProperties();
		priorities.resize(queueDscr.size());
		for (int i = 0; i < priorities.size(); i++) {
			auto& pri = priorities[i];
			pri.resize(queueDscr[i].queueCount);
			for (int j = 0; j < queueDscr[i].queueCount; j++) {
				pri[j] = 1.0f;
			}
		}
	}

	lv::Device& allocateDevice(lv::PhysicalDevice& physicalDevice, vk::DeviceCreateInfo& templ, std::vector<std::vector<float>>& priorities) {
		physicalDevice.devices.emplace_back();
		lv::Device& device = physicalDevice.devices.back();
		device.physicalDevice = &physicalDevice;

		std::vector<vk::DeviceQueueCreateInfo> queueInfos;

		queueInfos.resize(priorities.size());
		for (int i = 0; i < priorities.size(); i++) {
			auto& qInfo = queueInfos[i];
			qInfo.setQueueFamilyIndex(i)
				.setQueueCount(priorities[i].size())
				.setQueuePriorities(priorities[i]);
		}
		
		templ.setQueueCreateInfos(queueInfos);

		device.src = physicalDevice.src.createDevice(templ);

		device.queues.resize(priorities.size());
		for (int i = 0; i < priorities.size(); i++) {
			device.queues[i].resize(priorities[i].size());
			for (int j = 0; j < priorities[i].size(); j++) {
				auto& q = device.queues[i][j];
				q.src = device.src.getQueue(i, j);
				q.device = &device;
				q.QueueIndex = i;
				q.priority = priorities[i][j];
			}
		
		}

		device.loader.init(device.src);

		return device;
	}

	bool graphicQueue(const vk::QueueFamilyProperties& prop) {
		return (bool)(prop.queueFlags & vk::QueueFlagBits::eGraphics);
	}

	bool asyncComputeQueue(const vk::QueueFamilyProperties& prop) {
		return (prop.queueFlags & vk::QueueFlagBits::eCompute) && !(prop.queueFlags & vk::QueueFlagBits::eGraphics);
	}

	bool transferQueue(const vk::QueueFamilyProperties& prop) {
		return (prop.queueFlags & vk::QueueFlagBits::eTransfer) && 
			!(prop.queueFlags & vk::QueueFlagBits::eCompute) && !(prop.queueFlags & vk::QueueFlagBits::eGraphics);
	}

	std::map<infr::QueueFunction, int> getDeviceQueueIndex(lv::PhysicalDevice& device, 
		std::map<infr::QueueFunction, std::function<bool(const vk::QueueFamilyProperties&)>>& query) {
		const auto& queueProps = device.src.getQueueFamilyProperties();
		std::map<infr::QueueFunction, int> map;
		
		//A word on the complexity of DeviceQueues
		//Device queues are not as nicely segregated as one might hope
		//some queues are strictly optional
		//some queues only exist in conjunction with other functionalities (graphics)
		//some queues behave differently when other function are disabled (async compute vs compute)
		//so rather than being picky and exclusive, greedy is actually the closest we have
		//subtle beaviors, such as making trasnfer queue optional or explicit, can be refined by specifying other missing functionalities
		for (int i = 0; i < queueProps.size(); i++) { 
			const auto& qProp = queueProps[i];
			for (auto& kv : query) {
				auto& key = kv.first;
				auto& condition = kv.second;
				if (condition(qProp)) {
					map[key] = i;
				}
			}
		}
		return map;
	}

}