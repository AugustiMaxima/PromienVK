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
		//optional, can be removed for debloating
		std::map<std::string, std::vector<vk::PhysicalDevice>> physicalDeviceMap;
		//map to support multi utility, vector to support multi gpu
		std::map<std::string, std::vector<vk::Device>> deviceMap;
		std::map<std::string, std::vector<vk::Queue>> queueMap;

		//multiple viewport, for applications like VR
		std::vector<vk::SurfaceKHR> surfaces;
		std::vector<vk::SwapchainKHR> swapchains;

		//we'll leave the GraphicsPipeline part alone, as it is completely dependent on implementation
	public:
		Base(std::string config);
		virtual ~Base();
	};
}



#endif