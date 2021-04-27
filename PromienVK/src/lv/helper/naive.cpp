#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include "../../dbg/vLog.hpp"
#include "../../infr/type.hpp"
#include "../../core/devicePick.hpp"
#include "../../core/settings.hpp"
#include "../../core/swapchain.hpp"
#include "../Instance.hpp"
#include "../Present.hpp"
#include "../Device.hpp"
#include "../Pipeline.hpp"
#include "deviceLib.hpp"
#include "naive.hpp"

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

		instance.src = vk::createInstance(instInfo);

		vk::DynamicLoader dl;
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

		instance.loader.init(instance.src, vkGetInstanceProcAddr);

#if defined(_DEBUG)
		instance.debugMessenger = instance.src.createDebugUtilsMessengerEXT(msgInfo, nullptr, instance.loader);
#endif
	}

	void enumeratePhysicalDevices(lv::Instance& instance) {
		for (const auto& pDevice : instance.src.enumeratePhysicalDevices()) {
			lv::PhysicalDevice device;
			device.instance = &instance;
			device.src = pDevice;
			instance.physicalDevices.push_back(device);
		}
	}

	void initializeSurface(lv::Instance& instance, lv::Surface& surface, int width, int height, std::string& appName) {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		surface.window = glfwCreateWindow(width, height, appName.c_str(), nullptr, nullptr);
		VkSurfaceKHR sf;
		if (glfwCreateWindowSurface((VkInstance)(instance.src), surface.window, nullptr, &sf) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		surface.surface = vk::SurfaceKHR(sf);
		surface.instance = &instance;
	}

	lv::PhysicalDevice* collectRenderingDevice(lv::Instance& instance, lv::Surface& surface) {
		std::map<infr::DeviceFunction, std::function<int(const lv::PhysicalDevice&)>> criteria;
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		vk::DeviceCreateInfo graphics = vk::DeviceCreateInfo()
			.setPEnabledExtensionNames(deviceExtensions);
		criteria[infr::DeviceFunction::graphic] = [&surface, &graphics](const lv::PhysicalDevice& device) {
			using namespace core::dps;
			if(!presentReady(device.src, surface.surface)){
				return -1;
			}
			if (!deviceCompatible(device.src, graphics)) {
				return -1;
			}
			return sortByVRAM(device) + vulkanVersionSupport(device);
		};
		auto matchedDevices = getSuitableMatches(criteria, instance);
		int index = matchedDevices[infr::DeviceFunction::graphic][0];
		return &(instance.physicalDevices[index]);
	}

	lv::Device* allocateLogicalDevice(lv::PhysicalDevice& physicalDevice) {
		vk::DeviceCreateInfo graphic;

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		graphic.enabledExtensionCount = deviceExtensions.size();
		graphic.ppEnabledExtensionNames = deviceExtensions.data();
		
		std::vector<std::vector<float>> priorities;
		autoPopulatePriorities(physicalDevice, priorities);
		return &allocateDevice(physicalDevice, graphic, priorities);
	}

	void initializeSwapchain(lv::SwapChain& swapchain, lv::Device& device, lv::Surface& surf, core::DisplaySettings& display) {
		auto& grgpu = device.physicalDevice->src;
		auto& surface = surf.surface;
		display.format = core::spc::selectSurfaceFormat(grgpu, surface, display.format);
		display.present = core::spc::selectPresentMode(grgpu, surface, display.present);
		display.resolution = core::spc::chooseSwapExtent(grgpu, surface, display.resolution);
		vk::SurfaceCapabilitiesKHR cap = grgpu.getSurfaceCapabilitiesKHR(surface);

		int imageCount = cap.minImageCount + 1;
		if (imageCount > cap.maxImageCount && !cap.maxImageCount)
			imageCount = cap.maxImageCount;

		vk::SwapchainCreateInfoKHR info = vk::SwapchainCreateInfoKHR()
			.setMinImageCount(imageCount)
			.setImageFormat(display.format.format)
			.setImageColorSpace(display.format.colorSpace)
			.setImageExtent(display.resolution)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setImageSharingMode(vk::SharingMode::eExclusive)
			.setQueueFamilyIndexCount(0)
			.setPQueueFamilyIndices(nullptr)
			.setPreTransform(cap.currentTransform)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setPresentMode(display.present)
			.setClipped(true)
			.setSurface(surface);

		swapchain.device = &device;
		swapchain.swapchain = device.src.createSwapchainKHR(info);

		swapchain.images = device.src.getSwapchainImagesKHR(swapchain.swapchain);
		for (int i = 0; i < swapchain.images.size(); i++) {
			vk::ImageViewCreateInfo info = vk::ImageViewCreateInfo()
				.setImage(swapchain.images[i])
				.setViewType(vk::ImageViewType::e2D)
				.setFormat(display.format.format)
				.setComponents({ vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity })
				.setSubresourceRange(
					vk::ImageSubresourceRange()
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setBaseMipLevel(0)
					.setLevelCount(1)
					.setBaseArrayLayer(0)
					.setLayerCount(1)
				);
			swapchain.imageViews.push_back(device.src.createImageView(info));
		}
	}

	void initializeFrameBuffer(lv::FrameBuffer& framebuffer, lv::SwapChain& swapchain, lv::RenderPass& renderPass, core::DisplaySettings& display) {
		framebuffer.framebuffers.resize(swapchain.images.size());
		framebuffer.swapchain = &swapchain;
		framebuffer.renderPass = &renderPass;
		for (int i = 0; i < swapchain.imageViews.size(); i++) {
			std::array<vk::ImageView, 1> attachments = { swapchain.imageViews[i] };
			vk::FramebufferCreateInfo info = vk::FramebufferCreateInfo()
				.setRenderPass(renderPass.src)
				.setAttachments(attachments)
				.setWidth(display.resolution.width)
				.setHeight(display.resolution.height)
				.setLayers(1);
			framebuffer.framebuffers[i] = renderPass.device->src.createFramebuffer(info);
		}
	}

	void initializeFrameSynchronization(lv::FrameSynchronization& sync, lv::FrameBuffer& frameBuffer, int queueIndex, int framesInFlight) {
		int swapchainSize = frameBuffer.framebuffers.size();
		sync.framesInFlight = framesInFlight > 0 ? framesInFlight : swapchainSize;
		sync.frameFinished.resize(sync.framesInFlight);
		sync.imageAcquired.resize(sync.framesInFlight);
		sync.renderComplete.resize(sync.framesInFlight);
		sync.commandPools.resize(sync.framesInFlight);
		sync.framebuffer = &frameBuffer;
		vk::Device& device = frameBuffer.renderPass->device->src;
		for (int i = 0; i < sync.framesInFlight; i++) {
			sync.frameFinished[i] = device.createFence(vk::FenceCreateInfo()
				.setFlags(vk::FenceCreateFlagBits::eSignaled));
			sync.imageAcquired[i] = device.createSemaphore(vk::SemaphoreCreateInfo());
			sync.renderComplete[i] = device.createSemaphore(vk::SemaphoreCreateInfo());

			auto& commandPool = sync.commandPools[i];
			commandPool.src = device.createCommandPool(vk::CommandPoolCreateInfo()
				.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
				.setQueueFamilyIndex(queueIndex));
			commandPool.device= frameBuffer.renderPass->device;
			commandPool.queueIndex = queueIndex;
			commandPool.buffers.push_back(lv::CommandBuffer());
			commandPool.buffers[0].commandPool = &commandPool;
			commandPool.buffers[0].src = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo()
				.setCommandBufferCount(1)
				.setCommandPool(commandPool.src)
				.setLevel(vk::CommandBufferLevel::ePrimary))[0];
		}
		sync.frameLease.resize(swapchainSize, nullptr);
	}

}