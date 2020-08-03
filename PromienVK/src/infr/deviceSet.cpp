#include "deviceSet.hpp"

namespace infr {

	namespace dvs {
		//perhaps in the future we'd add RT Queues

		bool computeCompatible(vk::PhysicalDevice device) {
			auto qs = device.getQueueFamilyProperties();
			for (auto& q : qs) {
				if (q.queueFlags & vk::QueueFlagBits::eCompute) {
					return true;
				}
			}
			return false;
		}

		bool graphicCompatible(vk::PhysicalDevice device) {
			auto qs = device.getQueueFamilyProperties();
			for (auto& q : qs) {
				if (q.queueFlags & vk::QueueFlagBits::eGraphics) {
					return true;
				}
			}
			return false;
		}

		//It's important to perhaps quantify how device selection is performed
		//We make some fundamental assumptions:
		//The (discrete) gpu with the latest feature set is likely the best
		//One would not buy a new gpu that is worse
		//For compute & graphics, vram is a useful indicator of abilities
		int computeRank(vk::PhysicalDevice device1, vk::PhysicalDevice device2) {
			return device1.getMemoryProperties().memoryHeaps[0].size
				+ device1.getProperties().apiVersion
				- device2.getMemoryProperties().memoryHeaps[0].size
				- device2.getProperties().apiVersion;
		}

		int graphicRank(vk::PhysicalDevice device1, vk::PhysicalDevice device2) {
			return device1.getMemoryProperties().memoryHeaps[0].size
				+ device1.getProperties().apiVersion
				- device2.getMemoryProperties().memoryHeaps[0].size
				- device2.getProperties().apiVersion;
		}


		void registerDeviceSet(std::map<std::string, std::vector<vk::PhysicalDevice>>& pDeviceMap, vk::PhysicalDevice device) {
			if (computeCompatible(device)) {
				pDeviceMap["compute"].push_back(device);
			}
			if (graphicCompatible(device)) {
				pDeviceMap["graphic"].push_back(device);
			}
		}

		void rankDeviceEligibility(std::map<std::string, std::vector<vk::PhysicalDevice>>& pDeviceMap) {
			auto& computeList = pDeviceMap["compute"];
			auto& graphicList = pDeviceMap["graphic"];

			std::sort(computeList.begin(), computeList.end(), computeRank);
			std::sort(graphicList.begin(), graphicList.end(), graphicRank);
		}

	}
}