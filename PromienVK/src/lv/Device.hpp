#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <vulkan/vulkan.hpp>
#include <vector>

namespace lv {
	struct PhysicalDevice;
	struct Device;

	struct Queue {
		Device* device;
		int QueueIndex;
		vk::Queue queue;
	};

	struct Device {
		PhysicalDevice* physical;
		vk::Device device;
		std::vector<std::vector<Queue>> queues;
	};
}

#endif