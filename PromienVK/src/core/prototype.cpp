#define GLFW_INCLUDE_VULKAN
#include "../dbg/vLog.hpp"
#include "devicePick.hpp"
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
		physicalDeviceMap["*"] = instance.enumeratePhysicalDevices();
		dps::pickPhysicalDevices(physicalDeviceMap, surfaces[0]);
	}

	void Prototype::createLogicalDevices() {
		//TODO: Assert that there is a suitable device for all categories
	}

	void Prototype::createQueues(){
	}

	void Prototype::configureSwapChain() {

	}

	void Prototype::configureImageView() {

	}

	void Prototype::configureGraphicsPipeline() {

	}

	void Prototype::render() {

	}

	void Prototype::cleanup() {


		instance.destroySurfaceKHR(surfaces[0]);
#if defined(_DEBUG)
		instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
#endif
		instance.destroy();
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	Prototype::~Prototype() {

	}

}