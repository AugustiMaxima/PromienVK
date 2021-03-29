#ifndef DEVICE_LIB_HPP
#define DEVICE_LIB_HPP

#include <map>
#include <vector>
#include <functional>
#include <vulkan/vulkan.hpp>


namespace infr{
	enum class DeviceFunction;
	enum class QueueFunction;
}

namespace lv {
	struct Instance;
	struct PhysicalDevice;
	struct Surface;
	struct Device;
}

namespace vk {
	struct DeviceCreateInfo;
}

namespace lvl {
	std::map<infr::DeviceFunction, std::vector<int>> getSuitableMatches(
		const std::map<infr::DeviceFunction, std::function<int(const lv::PhysicalDevice&)>>& rankMap, 
		lv::Instance& instance);

	int sortByVRAM(const lv::PhysicalDevice& device);
	int vulkanVersionSupport(const lv::PhysicalDevice& device);

	void autoPopulatePriorities(lv::PhysicalDevice& physicalDevice, std::vector<std::vector<float>>& priorities);
	lv::Device& allocateDevice(lv::PhysicalDevice& physicalDevice, vk::DeviceCreateInfo& templ, std::vector<std::vector<float>>& priorities);


	bool graphicQueue(const vk::QueueFamilyProperties& prop);
	bool asyncComputeQueue(const vk::QueueFamilyProperties& prop);
	bool transferQueue(const vk::QueueFamilyProperties& prop);

	std::map<infr::QueueFunction, int> getDeviceQueueIndex(lv::PhysicalDevice& device, 
		std::map<infr::QueueFunction, std::function<bool(const vk::QueueFamilyProperties&)>>& query);
}

#endif