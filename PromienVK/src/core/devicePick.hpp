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

		//frustration free packaging
		struct deviceCreateEnclosure {
			vk::DeviceCreateInfo ref;
			//owing queue's specificy to devices, this might be hairy to get right
			//we may revisit this at a later time
			//std::vector<vk::DeviceQueueCreateInfo> queues;
			std::set<std::string> exts;
			std::set<std::string> lyrs;
			std::vector<const char*> extensions;
			std::vector<const char*> layers;
			vk::PhysicalDeviceFeatures deviceFeatures;
		};

		bool presentReady(vk::PhysicalDevice device, vk::SurfaceKHR surface);

		bool deviceCompatible(vk::PhysicalDevice device, vk::DeviceCreateInfo spec);

		void pickPhysicalDevices(std::map<std::string, std::vector<vk::PhysicalDevice>>& deviceMap,
			std::map<std::string, vk::DeviceCreateInfo>& templ, vk::SurfaceKHR surface);

		deviceCreateEnclosure iconoSynthesis(std::vector<vk::DeviceCreateInfo>& templs);

		std::map<std::string, std::vector<int>> physicalDeviceIndexing(std::map<std::string, std::vector<vk::PhysicalDevice>>& pDeviceMap);

		std::map<std::string, std::vector<bool>> naiveSelection(std::map<std::string, std::vector<int>>& deviceIndice);

		void pickDevices(std::map<std::string, std::vector<vk::PhysicalDevice>>& pdeviceMap, vk::SurfaceKHR surface, 
			std::map<std::string, std::vector<vk::Device>>& deviceMap, std::map<std::string, vk::DeviceCreateInfo>& templ,
			std::function<std::map<std::string, std::vector<bool>>(std::map<std::string, std::vector<int>>&)> selector = naiveSelection);
	
		vk::Device allocateDeviceQueue(vk::PhysicalDevice physicalDevice, vk::DeviceCreateInfo templat);
		
		void retrieveQueues(std::vector<vk::Device>& devices, std::vector<vk::PhysicalDevice>& deviceRef,
			std::map<vk::Device, std::map<std::string, util::multIndex<float, vk::Queue>>>& queueMap);

		std::map<std::string, util::multIndex<float, vk::Queue>> collectDeviceQueue(vk::Device device, vk::PhysicalDevice deviceRef);
	}
}


#endif