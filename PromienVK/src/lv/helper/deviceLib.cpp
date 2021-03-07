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
		const std::map<infr::DeviceFunction, std::function<int(lv::PhysicalDevice&)>>& rankMap,
		lv::Instance& instance, lv::Surface& surface) {
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
	int sortByVRAM(lv::PhysicalDevice& device) {
		return device.src.getMemoryProperties().memoryHeaps[0].size / 1048576;
	}

	int vulkanVersionSupport(lv::PhysicalDevice& device) {
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
		std::vector<std::vector<float>> priorities;

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
	}

}