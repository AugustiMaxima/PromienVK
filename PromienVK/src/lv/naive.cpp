#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "../dbg/vLog.hpp"
#include "naive.hpp"
#include "Instance.hpp"
#include "Present.hpp"
#include "Device.hpp"

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

		instance.instance = vk::createInstance(instInfo);

		vk::DynamicLoader dl;
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

		instance.loader.init(instance.instance, vkGetInstanceProcAddr);

#if defined(_DEBUG)
		instance.debugMessenger = instance.instance.createDebugUtilsMessengerEXT(msgInfo, nullptr, instance.loader);
#endif
	}

	void enumeratePhysicalDevices(lv::Instance& instance) {
		for (const auto& pDevice : instance.instance.enumeratePhysicalDevices()) {
			lv::PhysicalDevice device;
			device.instance = &instance;
			device.src = pDevice;
			instance.devices.push_back(device);
		}
	}

	void initializeSurface(lv::Instance& instance, lv::Surface& surface, int width, int height, std::string& appName) {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		surface.window = glfwCreateWindow(width, height, appName.c_str(), nullptr, nullptr);
		VkSurfaceKHR sf;
		if (glfwCreateWindowSurface((VkInstance)(instance.instance), surface.window, nullptr, &sf) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		surface.surface = vk::SurfaceKHR(sf);
		surface.instance = &instance;
	}

	lv::PhysicalDevice* collectRenderingDevice(lv::Instance& instance, lv::Surface& surface) {
		const auto& devices = instance.devices;
		return nullptr;
	}


}