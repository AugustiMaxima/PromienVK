#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <vulkan/vulkan.hpp>
#include <vector>
#include <map>

namespace lv {

	struct Instance;
	struct Device;

	struct PhysicalDevice {
		Instance* instance;
		vk::PhysicalDevice src;
		std::vector<Device> logicalDevices;
	};

	struct Instance {
		vk::Instance instance;
		std::vector<PhysicalDevice> devices;
	};
}

#endif
