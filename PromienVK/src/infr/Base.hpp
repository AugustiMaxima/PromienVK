#ifndef BASE_H
#define BASE_H

#include <vulkan/vulkan.hpp>
#include "Conf.hpp"
#include "InfraVK.hpp"
namespace infr {
	class Base : public InfraVK {
	private:
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
		std::map<std::string, std::vector<vk::Queue>> queueMap;
		std::map<std::string, std::vector<vk::DispatchLoaderDynamic>> dldm;

		//multiple viewport, for applications like VR
		std::vector<vk::SurfaceKHR> surfaces;
		std::vector<vk::SwapchainKHR> swapchains;

		//we'll leave the GraphicsPipeline part alone, as it is completely dependent on implementation
	public:
		Base(std::string config);
		//It's important to note that while data is general, implementation cannot
		//Implementations will strive to be a good reference for versatile uses
		//But constraints will be made
		virtual void createInstance();
		virtual void createSurface();
		virtual void allocatePhysicalDevices();
		virtual void createLogicalDevices();
		virtual void createQueues();
		virtual void configureSwapChain();
		virtual void configureImageView();
		virtual ~Base();
	};
}

#endif