#include "deviceSet.hpp"
#include "Base.hpp"

#if defined(_DEBUG)
#include "../dbg/vLog.hpp"
#endif


namespace infr {
	Base::Base(std::string config) :InfraVK() {
		conf::parseConfigs(configs, config);
	}

	void Base::createInstance() {
		conf::Scope appScope = configs["Application"];
		vk::ApplicationInfo appInfo = vk::ApplicationInfo()
			.setPApplicationName(((std::string)appScope["Title"]).c_str())
			.setApplicationVersion(appScope["AppVersion"])
			.setPEngineName("PromienVK")
			.setEngineVersion(0)
			.setApiVersion(VK_API_VERSION_1_2);

		// Use validation layers if this is a debug build
		std::vector<const char*> layers;

		//extensions:
		std::vector<const char*> extensions;

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

	void Base::allocatePhysicalDevices() {
		//global, includes all physicalDevices
		physicalDeviceMap["*"] = instance.enumeratePhysicalDevices();
		auto& deviceList = physicalDeviceMap["*"];

		for (auto& device : deviceList) {
			registerDeviceSet(physicalDeviceMap, device);
		}
		
		rankDeviceEligibility(physicalDeviceMap);
	
	}


	Base::~Base() {
		//TO BE COMPLETED
		instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
		instance.destroy();
	}



}