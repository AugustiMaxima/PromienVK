#include "../lv/helper/naive.hpp"
#include "../lv/helper/deviceLib.hpp"
#include "harness.hpp"

namespace core {

	Harness::Harness(const std::string& config) {

		conf::parseConfigs(configs, config);

		auto& appScope = configs["Application"];
		auto display = processDisplaySettings(configs["Display"]);

		std::string title = (std::string&)appScope["Title"];

		lvl::configureInstance(instance, title, VK_MAKE_VERSION(appScope["MajorVersion"], appScope["MinorVersion"], appScope["PatchVersion"]));
		lvl::enumeratePhysicalDevices(instance);
		lvl::initializeSurface(instance, surface, display.resolution.width, display.resolution.height, title);

		pDevice = lvl::collectRenderingDevice(instance, surface);

		lv::PhysicalDevice& physicalDevice = *pDevice;
		
		std::map<infr::QueueFunction, std::function<bool(const vk::QueueFamilyProperties&)>> query;
		query[infr::QueueFunction::graphic] = lvl::graphicQueue;
		query[infr::QueueFunction::compute] = lvl::asyncComputeQueue;
		query[infr::QueueFunction::transfer] = lvl::transferQueue;
		
		auto queueIndice = lvl::getDeviceQueueIndex(physicalDevice, query);


	}



}