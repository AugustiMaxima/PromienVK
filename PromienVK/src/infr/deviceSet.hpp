#ifndef DEVICESET_H
#define DEVICESET_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include <map>

namespace infr {
	void registerDeviceSet(std::map<std::string, std::vector<vk::PhysicalDevice>>& pDeviceMap, vk::PhysicalDevice device);
	void rankDeviceEligibility(std::map<std::string, std::vector<vk::PhysicalDevice>>& pDeviceMap);
}

#endif
