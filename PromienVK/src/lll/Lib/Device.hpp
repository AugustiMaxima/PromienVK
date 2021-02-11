#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <vulkan/vulkan.hpp>

namespace lv {
	struct PhysicalDevice;
	struct Device;

	struct Queue {
		Device& device;
		const int QueueIndex;
		vk::Queue queue;
	};

	struct Device {
		PhysicalDevice& physical;
		vk::Device device;
		std::vector<std::vector<vk::Queue>>
	};
}

#endif