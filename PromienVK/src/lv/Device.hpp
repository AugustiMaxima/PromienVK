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
		float priority;
		vk::Queue src;
	};

	struct Device {
		PhysicalDevice* physicalDevice;
		vk::Device src;
		vk::DispatchLoaderDynamic loader;
		std::vector<std::vector<Queue>> queues;
	};
}

#endif