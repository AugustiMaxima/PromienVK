#include "deviceLib.hpp"
#include <vector>
#include <map>
#include <functional>
#include "../infr/type.hpp"
#include "../utils/heap.hpp"
#include "Instance.hpp"

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

}