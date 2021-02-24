#ifndef INSTANCE_HPP
#define INSTANCE_HPP

//#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <vector>
#include <map>
#include "Device.hpp"

//VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace lv {

	struct Instance;

	struct PhysicalDevice {
		Instance* instance;
		vk::PhysicalDevice src;
		std::vector<Device> logicalDevices;
	};

	struct Instance {
		vk::Instance instance;
		std::vector<PhysicalDevice> devices;
		vk::DispatchLoaderDynamic loader;
#if defined(_DEBUG)
		vk::DebugUtilsMessengerEXT debugMessenger;
#endif
	};
}

#endif
