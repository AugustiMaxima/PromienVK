#ifndef PROTOTYPE
#define PROTOTYPE

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "../infr/InfraVk.hpp"
#include "../infr/Conf.hpp"
#include "../infr/type.hpp"
#include "settings.hpp"

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
		settings::DisplaySettings display;
		vk::Extent2D resolution;
		std::vector<vk::Image> swapchainImages;
		std::vector<vk::ImageView> swapchainImageViews;

		vk::RenderPass renderPass;
		vk::PipelineLayout pipelineLayout;
		vk::Pipeline pipeline;

		std::vector<vk::Framebuffer> framebuffers;

		std::map<infr::QueueFunction, std::vector<vk::CommandPool>> commandPools;

		virtual void createInstance();
		virtual void createSurface();
		virtual void allocatePhysicalDevices();
		virtual void createLogicalDevices();
		virtual void configureSwapChain();
		virtual void configureImageView();
		virtual void configureRenderPass();
		virtual void configureGraphicsPipeline();
		virtual void configureFramebuffers();
		virtual void configureCommandPool();
		virtual void render();
		virtual void cleanup();

	public:
		Prototype(std::string config);
		virtual ~Prototype();
	};
}

#endif