#ifndef DEVICEPICK_H
#define DEVICEPICK_H

#include <vulkan/vulkan.hpp>
#include <string>
#include <map>
#include <set>
#include <functional>
#include "../utils/multIndex.hpp"

namespace core {
	namespace dps {
		bool presentReady(vk::PhysicalDevice device, vk::SurfaceKHR surface);
		int presentScore(vk::PhysicalDevice device1, vk::PhysicalDevice device2);

		void pickPhysicalDevices(std::map<std::string, std::vector<vk::PhysicalDevice>>& deviceMap, vk::SurfaceKHR surface);

		std::map<std::string, std::vector<int>> naiveSelection(std::map<std::string, std::vector<vk::PhysicalDevice>>& pDeviceMap);

		void pickDevices(std::map<std::string, std::vector<vk::PhysicalDevice>>& pdeviceMap,
			vk::SurfaceKHR surface, std::map<std::string, std::vector<vk::Device>>& deviceMap,
			std::map<std::string, util::multIndex<float, vk::Queue>>& queueMap,
			std::function<std::map<std::string, std::vector<int>>(std::map<std::string, std::vector<vk::PhysicalDevice>>& pDeviceMap)> = naiveSelection);
	}
}


#endif