#ifndef DEVICESET_H
#define DEVICESET_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include <map>
#include "type.hpp"

namespace infr {
	namespace dvs {
		bool computeCompatible(vk::PhysicalDevice device);
		bool graphicCompatible(vk::PhysicalDevice device);
		int computeRank(vk::PhysicalDevice device1, vk::PhysicalDevice device2);
		int graphicRank(vk::PhysicalDevice device1, vk::PhysicalDevice device2);

		void registerDeviceSet(std::map<DeviceFunction, std::vector<vk::PhysicalDevice>>& pDeviceMap, vk::PhysicalDevice device);
		void rankDeviceEligibility(std::map<DeviceFunction, std::vector<vk::PhysicalDevice>>& pDeviceMap);
	}
}

#endif
