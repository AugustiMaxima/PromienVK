#ifndef PROTOTYPE
#define PROTOTYPE

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "../infr/InfraVk.hpp"
#include "../infr/Conf.hpp"
#include "../infr/type.hpp"

namespace core {
	class Prototype : public infr::InfraVK {
		void initWindow();
	protected:
		//We have integrated GLFW to provide cross plat compatibility
		conf::Scope configs;
		vk::Instance instance;
		vk::DispatchLoaderDynamic dldi;

#if defined(_DEBUG)
		vk::DebugUtilsMessengerEXT debugMessenger;
#endif

		GLFWwindow* window;
		vk::SurfaceKHR surface;
		vk::PhysicalDevice grgpu;
		vk::Device device;
		vk::DispatchLoaderDynamic dldd;
		std::map<infr::QueueFunction, std::vector<vk::Queue>> queueMap;
		vk::SwapchainKHR swapchain;
		std::vector<vk::Image> swapchainImages;
		std::vector<vk::ImageView> swapchainImageViews;

		virtual void createInstance();
		virtual void createSurface();
		virtual void allocatePhysicalDevices();
		virtual void createLogicalDevices();
		virtual void configureSwapChain();
		virtual void configureImageView();
		virtual void configureGraphicsPipeline();
		virtual void render();
		virtual void cleanup();

	public:
		Prototype(std::string config);
		virtual ~Prototype();
	};
}

#endif