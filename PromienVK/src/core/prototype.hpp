#ifndef PROTOTYPE
#define PROTOTYPE

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "../infr/InfraVk.hpp"
#include "../infr/Conf.hpp"
#include "../infr/type.hpp"
#include "../utils/semaphore.hpp"
#include "settings.hpp"
#include "../asset/io/loader.hpp"
#include "../asset/format/obj.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

//Deprecated in favor of harness.hpp

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
		std::map<infr::QueueFunction, int> queueIndex;
		std::map<infr::QueueFunction, std::vector<vk::Queue>> queueMap;
		vk::SwapchainKHR swapchain;
		DisplaySettings display;
		std::vector<vk::Image> swapchainImages;
		std::vector<vk::ImageView> swapchainImageViews;

		vk::RenderPass renderPass;
		vk::PipelineLayout pipelineLayout;
		vk::Pipeline pipeline;

		std::vector<vk::Framebuffer> framebuffers;

		std::vector<vk::CommandPool> commandPools;
		std::vector<vk::CommandBuffer> commandBuffers;

		int maxFramesLatency;

		std::vector<vk::Semaphore> imgAcquired;
		std::vector<vk::Semaphore> rndrFinished;

		std::vector<vk::Fence> frameFinished;
		std::vector<vk::Fence> imageLease;


		asset::io::StreamHost host;
		asset::format::obj* lux;

		asset::io::StageVueue stageBuffer;
		asset::io::Vueue vram;

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
		virtual void configureSynchronization();
		virtual void configureAssets();
		virtual void setup();
		virtual void render();
		virtual void renderFrame(unsigned f);
		virtual void cleanup();

	public:
		Prototype(std::string config);
		virtual ~Prototype();
	};
}

#endif