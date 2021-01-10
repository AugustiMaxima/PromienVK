#ifndef DEVICEPICK_H
#define DEVICEPICK_H

#include <vulkan/vulkan.hpp>
#include <string>
#include <map>
#include <set>
#include <functional>
#include "../infr/type.hpp"
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

		bool computeRankCmp(vk::PhysicalDevice d1, vk::PhysicalDevice d2);
		bool graphicRankCmp(vk::PhysicalDevice d1, vk::PhysicalDevice d2);

		bool presentReady(vk::PhysicalDevice device, vk::SurfaceKHR surface);

		bool deviceCompatible(vk::PhysicalDevice device, vk::DeviceCreateInfo spec);

		void pickPhysicalDevices(std::map<infr::DeviceFunction, std::vector<vk::PhysicalDevice>>& deviceMap,
			std::map<infr::DeviceFunction, vk::DeviceCreateInfo>& templ, vk::SurfaceKHR surface);

		deviceCreateEnclosure iconoSynthesis(std::vector<vk::DeviceCreateInfo>& templs);

		std::map<infr::DeviceFunction, std::vector<int>> physicalDeviceIndexing(std::map<infr::DeviceFunction, std::vector<vk::PhysicalDevice>>& pDeviceMap);

		std::map<infr::DeviceFunction, std::vector<bool>> naiveSelection(std::map<infr::DeviceFunction, std::vector<int>>& deviceIndice);

		void pickDevices(std::map<infr::DeviceFunction, std::vector<vk::PhysicalDevice>>& pdeviceMap, vk::SurfaceKHR surface,
			std::map<infr::DeviceFunction, std::vector<vk::Device>>& deviceMap, std::map<infr::DeviceFunction, vk::DeviceCreateInfo>& templ,
			std::map<infr::QueueFunction, std::function<bool(vk::QueueFamilyProperties)>>& query,
			std::function<std::map<infr::DeviceFunction, std::vector<bool>>(std::map<infr::DeviceFunction, std::vector<int>>&)> selector = naiveSelection);
		
		vk::Device allocateDeviceQueue(vk::PhysicalDevice physicalDevice, vk::DeviceCreateInfo templat,
			std::map<infr::QueueFunction, std::function<bool(vk::QueueFamilyProperties)>>& query);
		
		void retrieveQueues(std::vector<vk::Device>& devices, std::vector<vk::PhysicalDevice>& deviceRef,
			std::map<infr::QueueFunction, std::function<bool(vk::QueueFamilyProperties)>>& query,
			std::map<vk::Device, std::map<infr::QueueFunction, util::multIndex<float, vk::Queue>>>& queueMap);

		std::map<infr::QueueFunction, util::multIndex<float, vk::Queue>> collectDeviceQueue(vk::Device device, vk::PhysicalDevice deviceRef, 
			std::map<infr::QueueFunction, std::function<bool(vk::QueueFamilyProperties)>>& query);
	
		std::map<infr::QueueFunction, int> collectDeviceQueueIndex(vk::PhysicalDevice device,
			std::map<infr::QueueFunction, std::function<bool(vk::QueueFamilyProperties)>>& query);

		std::map<infr::QueueFunction, util::multIndex<float, vk::Queue>> fetchDeviceQueue(vk::PhysicalDevice pDevice, vk::Device device, std::map<infr::QueueFunction, int>& index);
	}
}


#endif