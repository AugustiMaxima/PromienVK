#ifndef DEVICEPICK_H
#define DEVICEPICK_H

#include <vulkan/vulkan.hpp>
#include <string>
#include <map>

namespace core {
	namespace dps {
		bool presentReady(vk::PhysicalDevice device);
		int presentScore(vk::PhysicalDevice device1, vk::PhysicalDevice device2);

		void pickDevices(std::map<std::string, std::vector<vk::PhysicalDevice>>& deviceMap, vk::SurfaceKHR surface);
	}
}


#endif