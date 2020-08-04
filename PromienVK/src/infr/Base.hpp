#ifndef BASE_H
#define BASE_H

#include <vulkan/vulkan.hpp>
#include "Conf.hpp"
#include "InfraVK.hpp"
#include "../utils/multIndex.hpp"
namespace infr {
	class Base : public InfraVK {
	protected:
		//Data is going to be inherited, so absolute care is needed for extensibility
		conf::Scope configs;
		vk::Instance instance;
		vk::DispatchLoaderDynamic dldi;

#if defined(_DEBUG)
		vk::DebugUtilsMessengerEXT debugMessenger;
#endif
		//optional, can be removed for debloating
		std::map<std::string, std::vector<vk::PhysicalDevice>> physicalDeviceMap;
		//map to support multi utility, vector to support multi gpu
		std::map<std::string, std::vector<vk::Device>> deviceMap;
		std::map<vk::Device, std::map<std::string, util::multIndex<float, vk::Queue>>> queueMap;
		std::map<std::string, std::vector<vk::DispatchLoaderDynamic>> dldm;

		//multiple viewport, for applications like VR
		std::vector<vk::SurfaceKHR> surfaces;
		std::vector<vk::SwapchainKHR> swapchains;
		std::vector<vk::Pipeline> pipelines;

		//we'll leave the GraphicsPipeline part alone, as it is completely dependent on implementation

		//It's important to note that while data is general, implementation cannot
		//Implementations will strive to be a good reference for versatile uses
		//But constraints will be made
		virtual void createInstance();
		virtual void allocatePhysicalDevices();
		virtual void cleanup();
	public:
		Base(std::string config);
		virtual ~Base();
	};
}

#endif