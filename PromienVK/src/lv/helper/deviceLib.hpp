#ifndef DEVICE_LIB_HPP
#define DEVICE_LIB_HPP

#include <map>
#include <vector>
#include <functional>

namespace infr{
	enum class DeviceFunction;
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
		const std::map<infr::DeviceFunction, std::function<int(lv::PhysicalDevice&)>>& rankMap, 
		lv::Instance& instance);

	int sortByVRAM(lv::PhysicalDevice& device);
	int vulkanVersionSupport(lv::PhysicalDevice& device);

	void autoPopulatePriorities(lv::PhysicalDevice& physicalDevice, std::vector<std::vector<float>>& priorities);
	lv::Device& allocateDevice(lv::PhysicalDevice& physicalDevice, vk::DeviceCreateInfo& templ, std::vector<std::vector<float>>& priorities);
}

#endif