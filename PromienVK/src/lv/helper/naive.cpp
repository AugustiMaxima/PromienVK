#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "../../dbg/vLog.hpp"
#include "../../infr/type.hpp"
#include "../../core/devicePick.hpp"
#include "../Instance.hpp"
#include "../Present.hpp"
#include "../Device.hpp"
#include "deviceLib.hpp"
#include "naive.hpp"

namespace lvl {
	void configureInstance(lv::Instance& instance, std::string& appName, uint32_t version) {
		vk::ApplicationInfo appInfo = vk::ApplicationInfo()
			.setPApplicationName(appName.c_str())
			.setApplicationVersion(version)
			.setPEngineName("PromienVK")
			.setEngineVersion(VK_MAKE_VERSION(0, 0, 0))
			.setApiVersion(VK_API_VERSION_1_2);

		std::vector<const char*> layers;

		//extensions:
		std::vector<const char*> extensions;

		uint32_t glfwExtensionCount;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		extensions.resize(glfwExtensionCount);
		for (int i = 0; i < extensions.size(); i++) {
			extensions[i] = glfwExtensions[i];
		}

#if defined(_DEBUG)
		layers.push_back("VK_LAYER_KHRONOS_validation");
		extensions.push_back("VK_EXT_debug_utils");
#endif

		vk::InstanceCreateInfo instInfo = vk::InstanceCreateInfo()
			.setFlags(vk::InstanceCreateFlags())
			.setPApplicationInfo(&appInfo)
			.setEnabledExtensionCount(extensions.size())
			.setPpEnabledExtensionNames(extensions.data())
			.setEnabledLayerCount(layers.size())
			.setPpEnabledLayerNames(layers.data());

#if defined(_DEBUG)
		vk::DebugUtilsMessengerCreateInfoEXT msgInfo = vk::DebugUtilsMessengerCreateInfoEXT()
			.setFlags(vk::DebugUtilsMessengerCreateFlagsEXT())
			.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
			.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
			.setPfnUserCallback((PFN_vkDebugUtilsMessengerCallbackEXT)dbg::debugCallBack)
			.setPNext(nullptr);
		instInfo.setPNext(&msgInfo);
#endif

		instance.src = vk::createInstance(instInfo);

		vk::DynamicLoader dl;
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

		instance.loader.init(instance.src, vkGetInstanceProcAddr);

#if defined(_DEBUG)
		instance.debugMessenger = instance.src.createDebugUtilsMessengerEXT(msgInfo, nullptr, instance.loader);
#endif
	}

	void enumeratePhysicalDevices(lv::Instance& instance) {
		for (const auto& pDevice : instance.src.enumeratePhysicalDevices()) {
			lv::PhysicalDevice device;
			device.instance = &instance;
			device.src = pDevice;
			instance.physicalDevices.push_back(device);
		}
	}

	void initializeSurface(lv::Instance& instance, lv::Surface& surface, int width, int height, std::string& appName) {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		surface.window = glfwCreateWindow(width, height, appName.c_str(), nullptr, nullptr);
		VkSurfaceKHR sf;
		if (glfwCreateWindowSurface((VkInstance)(instance.src), surface.window, nullptr, &sf) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		surface.surface = vk::SurfaceKHR(sf);
		surface.instance = &instance;
	}

	lv::PhysicalDevice* collectRenderingDevice(lv::Instance& instance, lv::Surface& surface) {
		std::map<infr::DeviceFunction, std::function<int(lv::PhysicalDevice&)>> criteria;
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		vk::DeviceCreateInfo graphics = vk::DeviceCreateInfo()
			.setPEnabledExtensionNames(deviceExtensions);
		criteria[infr::DeviceFunction::graphic] = [&surface, &graphics](lv::PhysicalDevice& device) {
			using namespace core::dps;
			if(!presentReady(device.src, surface.surface)){
				return -1;
			}
			if (!deviceCompatible(device.src, graphics)) {
				return -1;
			}
			return sortByVRAM(device) + vulkanVersionSupport(device);
		};
		auto matchedDevices = getSuitableMatches(criteria, instance);
		int index = matchedDevices[infr::DeviceFunction::graphic][0];
		return &(instance.physicalDevices[index]);
	}

	lv::Device* allocateLogicalDevice(lv::PhysicalDevice& physicalDevice) {
		vk::DeviceCreateInfo graphic;

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		graphic.enabledExtensionCount = deviceExtensions.size();
		graphic.ppEnabledExtensionNames = deviceExtensions.data();
		
		std::vector<std::vector<float>> priorities;
		autoPopulatePriorities(physicalDevice, priorities);
		return &allocateDevice(physicalDevice, graphic, priorities);
	}


}