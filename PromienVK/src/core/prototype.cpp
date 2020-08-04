#define GLFW_INCLUDE_VULKAN
#include "../dbg/vLog.hpp"
#include "devicePick.hpp"
#include "swapchain.hpp"
#include "settings.hpp"
#include "prototype.hpp"

namespace core {

	Prototype::Prototype(std::string config):infr::Base(config){}

	void Prototype::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		//TODO: (bordleress)\ fullscreen support
		conf::Scope display = configs["Display"];
		conf::Scope appScope = configs["Application"];
		window = glfwCreateWindow(display["XRes"], display["YRes"], ((std::string)appScope["Title"]).c_str(), nullptr, nullptr);
	}

	void Prototype::createInstance() {
		initWindow();

		conf::Scope appScope = configs["Application"];
		vk::ApplicationInfo appInfo = vk::ApplicationInfo()
			.setPApplicationName(((std::string)appScope["Title"]).c_str())
			.setApplicationVersion(VK_MAKE_VERSION(appScope["MajorVersion"], appScope["MinorVersion"], appScope["PatchVersion"]))
			.setPEngineName("PromienVK")
			.setEngineVersion(VK_MAKE_VERSION(0,0,0))
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

		if (vk::createInstance(&instInfo, nullptr, &instance) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create instance!");
		}

		dldi.init(instance);

#if defined(_DEBUG)
		debugMessenger = instance.createDebugUtilsMessengerEXT(msgInfo, nullptr, dldi);
#endif
	}

	void Prototype::createSurface() {
		VkSurfaceKHR surface;
		//Use .hpp they said, it will clean up everything, they said
		if (glfwCreateWindowSurface((VkInstance)instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		surfaces.push_back(vk::SurfaceKHR(surface));
	}

	void Prototype::allocatePhysicalDevices() {
		std::map<std::string, vk::DeviceCreateInfo> templ;
		vk::DeviceCreateInfo& graphic = templ["graphic"];
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		graphic.enabledExtensionCount = deviceExtensions.size();
		graphic.ppEnabledExtensionNames = deviceExtensions.data();

#if defined(_DEBUG)
		//Fill in layer data for legacy vk implementations
#endif
		physicalDeviceMap["*"] = instance.enumeratePhysicalDevices();
		dps::pickPhysicalDevices(physicalDeviceMap, templ, surfaces[0]);

		if (!physicalDeviceMap["graphic"].size())
			throw std::runtime_error("No suitable gpu found for this application");
	}

	void Prototype::createLogicalDevices() {
		std::map<std::string, vk::DeviceCreateInfo> templ;
		//configure extension count
		//pattern: use template to configure device properties, can be shared across physical and logical device selection
		vk::DeviceCreateInfo& graphic = templ["graphic"];
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		graphic.enabledExtensionCount = deviceExtensions.size();
		graphic.ppEnabledExtensionNames = deviceExtensions.data();

		dps::pickDevices(physicalDeviceMap, surfaces[0], deviceMap, templ);

		dps::retrieveQueues(deviceMap["*"], physicalDeviceMap["*"], queueMap);
	}

	void Prototype::configureSwapChain() {
		settings::DisplaySettings display = settings::processDisplaySettings(configs["Display"]);
		display.format = spc::selectSurfaceFormat(physicalDeviceMap["graphic"][0], surfaces[0], display.format);
		display.present = spc::selectPresentMode(physicalDeviceMap["graphic"][0], surfaces[0], display.present);
		display.resolution = spc::chooseSwapExtent(physicalDeviceMap["graphic"][0], surfaces[0], display.resolution);
		settings::updateDisplaySettings(configs["Display"], display);

		//TODO: Actually construct the damn swap chain
	}

	void Prototype::configureImageView() {

	}

	void Prototype::configureGraphicsPipeline() {

	}

	void Prototype::render() {

	}

	void Prototype::cleanup() {
		//TODO: destroy physical and logical devices

		instance.destroySurfaceKHR(surfaces[0]);
#if defined(_DEBUG)
		instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
#endif
		instance.destroy();
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	Prototype::~Prototype() {
		cleanup();
	}

}