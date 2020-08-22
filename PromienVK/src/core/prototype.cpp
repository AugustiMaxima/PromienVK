#define GLFW_INCLUDE_VULKAN
#include "../dbg/vLog.hpp"
#include "../infr/deviceSet.hpp"
#include "devicePick.hpp"
#include "../utils/multindex.hpp"
#include "swapchain.hpp"
#include "shader.hpp"
#include "pipeline.hpp"
#include "boilerplate.hpp"
#include "renderpass.hpp"
#include "prototype.hpp"

namespace core {

	Prototype::Prototype(std::string config){
		conf::parseConfigs(configs, config);
	}

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

		vk::DynamicLoader dl;
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

		dldi.init(instance, vkGetInstanceProcAddr);

#if defined(_DEBUG)
		debugMessenger = instance.createDebugUtilsMessengerEXT(msgInfo, nullptr, dldi);
#endif
	}

	void Prototype::createSurface() {
		VkSurfaceKHR sf;
		//Use .hpp they said, it will clean up everything, they said
		if (glfwCreateWindowSurface((VkInstance)instance, window, nullptr, &sf) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		surface = vk::SurfaceKHR(sf);
	}

	void Prototype::allocatePhysicalDevices() {
		std::map<infr::DeviceFunction, vk::DeviceCreateInfo> templ;
		vk::DeviceCreateInfo& graphic = templ[infr::DeviceFunction::graphic];
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		graphic.enabledExtensionCount = deviceExtensions.size();
		graphic.ppEnabledExtensionNames = deviceExtensions.data();

#if defined(_DEBUG)
		//Fill in layer data for legacy vk implementations
#endif
		std::map<infr::DeviceFunction, std::vector<vk::PhysicalDevice>> physicalDeviceMap;
		std::map<infr::DeviceFunction, std::vector<vk::Device>> deviceMap;

		physicalDeviceMap[infr::DeviceFunction::all] = instance.enumeratePhysicalDevices();
		dps::pickPhysicalDevices(physicalDeviceMap, templ, surface);

		if (!physicalDeviceMap[infr::DeviceFunction::graphic].size())
			throw std::runtime_error("No suitable gpu found for this application");

		grgpu = physicalDeviceMap[infr::DeviceFunction::all][0];
	}

	void Prototype::createLogicalDevices() {
		vk::DeviceCreateInfo graphic;

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		graphic.enabledExtensionCount = deviceExtensions.size();
		graphic.ppEnabledExtensionNames = deviceExtensions.data();

#if defined(_DEBUG)
		//Fill in layer data for legacy vk implementations
#endif
		std::map<infr::QueueFunction, std::function<bool(vk::QueueFamilyProperties)>> queuery;
		queuery[infr::QueueFunction::graphic] = infr::dvs::isGraphicQueue;
		queuery[infr::QueueFunction::compute] = infr::dvs::isAsyncCompute;
		queuery[infr::QueueFunction::transfer] = infr::dvs::isTransferQueue;

		device = dps::allocateDeviceQueue(grgpu, graphic, queuery);

		std::map<infr::QueueFunction, util::multIndex<float, vk::Queue>> indexedMap = dps::collectDeviceQueue(device, grgpu, queuery);
		for (const auto& index : indexedMap) {
			util::multIndex<float, vk::Queue> queues = index.second;
			queueMap[index.first] = queues.query(1.0f, 1.0f);
		}
	}

	void Prototype::configureSwapChain() {
		display = settings::processDisplaySettings(configs["Display"]);
		display.format = spc::selectSurfaceFormat(grgpu, surface, display.format);
		display.present = spc::selectPresentMode(grgpu, surface, display.present);
		display.resolution = spc::chooseSwapExtent(grgpu, surface, display.resolution);
		settings::updateDisplaySettings(configs["Display"], display);

		//TODO: Actually construct the damn swap chain

		vk::SurfaceCapabilitiesKHR cap = grgpu.getSurfaceCapabilitiesKHR(surface);

		int imageCount = cap.minImageCount + 1;

		if (imageCount > cap.maxImageCount && !cap.maxImageCount)
			imageCount = cap.maxImageCount;

		//TODO: update eColorattachedment to Transfer
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
		//TODO: old Swap Chain for resizing ops

		swapchain = device.createSwapchainKHR(info);
	}

	void Prototype::configureImageView() {
		settings::DisplaySettings display = settings::processDisplaySettings(configs["Display"]);
		swapchainImages = device.getSwapchainImagesKHR(swapchain);
		for (int i = 0; i < swapchainImages.size(); i++) {
			vk::ImageViewCreateInfo info = vk::ImageViewCreateInfo()
				.setImage(swapchainImages[i])
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
			swapchainImageViews.push_back(device.createImageView(info));
		}
	}

	void Prototype::configureRenderPass() {
		pipeline::RenderPassEnclosure rend;
		rend.attachments.push_back(pipeline::configureColorAttachment(display.format.format));
		rend.subpassInf.resize(1);
		rend.subpassInf[0].color.push_back(vk::AttachmentReference()
			.setAttachment(0)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
		);
		rend.dependencies.push_back(vk::SubpassDependency()
			.setSrcSubpass(VK_SUBPASS_EXTERNAL)
			.setDstSubpass(0)
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite));

		//configure subpass dependencies

		renderPass = rend.construct(device);
	}

	void Prototype::configureGraphicsPipeline() {
		pipeline::GraphicsPipelineEnclosure pip = pipeline::configureGraphicsPipeline(vk::PrimitiveTopology::eTriangleList, false, display.resolution);
		pip.shaders.entryName = {"main","main"};
		pip.shaders.srcs = { "shaders/shader.vert.spv", "shaders/shader.frag.spv" };
		pip.shaders.stages = { vk::ShaderStageFlagBits::eVertex , vk::ShaderStageFlagBits::eFragment };

		pipeline = pip.construct(device, renderPass, 0, nullptr, -1, nullptr);
		pipelineLayout = pip.uniform.layout;

		for (auto& shader : pip.shaders.shaders) {
			device.destroy(shader);
		}
	}

	void Prototype::configureFramebuffers() {
		framebuffers.resize(swapchainImages.size());
		for (int i = 0; i < swapchainImages.size(); i++) {
			vk::ImageView attachments[] = { swapchainImageViews[i] };
			vk::FramebufferCreateInfo info = vk::FramebufferCreateInfo()
				.setRenderPass(renderPass)
				.setAttachmentCount(1)
				.setPAttachments(attachments)
				.setWidth(display.resolution.width)
				.setHeight(display.resolution.height)
				.setLayers(1);
			framebuffers[i] = device.createFramebuffer(info);
		}
	}
	
	void Prototype::configureCommandPool() {
		std::map<infr::QueueFunction, std::function<bool(vk::QueueFamilyProperties)>> queuery;
		queuery[infr::QueueFunction::graphic] = infr::dvs::isGraphicQueue;
		std::map<infr::QueueFunction, int> result = dps::collectDeviceQueueIndex(grgpu, queuery);
	
		auto info = vk::CommandPoolCreateInfo()
			.setQueueFamilyIndex(result[infr::QueueFunction::graphic])
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
		
		for (int i = 0; i < swapchainImages.size(); i++) {
			commandPools.push_back(device.createCommandPool(info));
			vk::CommandBufferAllocateInfo info = vk::CommandBufferAllocateInfo()
				.setCommandBufferCount(1)
				.setCommandPool(commandPools[i])
				.setLevel(vk::CommandBufferLevel::ePrimary);
			commandBuffers.push_back(device.allocateCommandBuffers(info)[0]);
		}
	}

	void Prototype::configureSynchronization() {
		maxFramesLatency = 3;
		imgAcquired.resize(maxFramesLatency);
		rndrFinished.resize(maxFramesLatency);
		frameFinished.resize(maxFramesLatency);
		imageLease.resize(swapchainImages.size(), nullptr);
		for (int i = 0; i < maxFramesLatency; i++) {
			imgAcquired[i] = device.createSemaphore(vk::SemaphoreCreateInfo());
			rndrFinished[i] = device.createSemaphore(vk::SemaphoreCreateInfo());
			frameFinished[i] = device.createFence(vk::FenceCreateInfo());
		}
	}

	void Prototype::setup() {
		createInstance();
		createSurface();
		allocatePhysicalDevices();
		createLogicalDevices();
		configureSwapChain();
		configureImageView();
		configureRenderPass();
		configureGraphicsPipeline();
		configureFramebuffers();
		configureCommandPool();
	}

	void Prototype::render() {
		configureSynchronization();
		unsigned fc = 0;

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			renderFrame(fc++);
		}

	}

	void Prototype::renderFrame(unsigned f) {
		unsigned id = f % swapchainImages.size();
		unsigned il = f % maxFramesLatency;
		device.waitForFences(frameFinished[il], true, UINT64_MAX);
		device.resetCommandPool(commandPools[id], vk::CommandPoolResetFlagBits::eReleaseResources);		

		if (imageLease[id]) {
			device.waitForFences(imageLease[id], true, UINT64_MAX);
		}

		//a note on this reset and the imageLease
		//this is a plug gap solution to solve the scenario where maxFramesLatency != swapchain image size
		//however, it is not perfect, and leads to defects (back-frame stutters) when there are more images than frames in flight
		//when possible, we should fix frame in flight to be equivalent to swapchain images, this eliminates the need for image leases
		//failing that, we should avoid making frame in flight lesser than swapchain images, this lead to nasty stale induced stutter
		device.resetFences(frameFinished[il]);
		device.acquireNextImageKHR(swapchain, UINT64_MAX, imgAcquired[il], nullptr, &id);


		//TODO: refactor and clean up boilerplate for info settings
		auto& cb = commandBuffers[id];
		cb.begin(vk::CommandBufferBeginInfo());
		std::array<float, 4> cn = { 0,0,0,1 };
		vk::ClearValue clearColor = cn;
		cb.beginRenderPass(vk::RenderPassBeginInfo()
			.setClearValueCount(1)
			.setPClearValues(&clearColor)
			.setRenderPass(renderPass)
			.setRenderArea(vk::Rect2D()
			.setExtent(display.resolution)
				.setOffset({ 0,0 }))
			.setFramebuffer(framebuffers[id])
			, vk::SubpassContents::eInline);
		cb.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
		cb.draw(3, 1, 0, 0);
		cb.endRenderPass();
		cb.end();

		auto& gqs = queueMap[infr::QueueFunction::graphic];
		auto& gq = gqs[f % gqs.size()];
		//this may be entirely unnecessary if not counterproductive, we may remove the load balancing if this appears perverse

		vk::PipelineStageFlags wstage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		auto submit = vk::SubmitInfo()
			.setCommandBufferCount(1)
			.setPCommandBuffers(&commandBuffers[id])
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(&rndrFinished[il])
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&imgAcquired[il])
			.setPWaitDstStageMask(&wstage);

		gq.submit(submit, frameFinished[il]);
		
		auto present = vk::PresentInfoKHR()
			.setSwapchainCount(1)
			.setPSwapchains(&swapchain)
			.setPImageIndices(&id)
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&rndrFinished[il]);

		gq.presentKHR(present);
	}


	void Prototype::cleanup() {

		for (int i = 0; i < maxFramesLatency; i++) {
			device.destroySemaphore(imgAcquired[i]);
			device.destroySemaphore(rndrFinished[i]);
			device.destroyFence(frameFinished[i]);
		}

		for (auto& cp : commandPools) {
			device.destroyCommandPool(cp);
		}

		for (auto& fb : framebuffers) {
			device.destroyFramebuffer(fb);
		}
		for (auto& imageView : swapchainImageViews) {
			device.destroyImageView(imageView);
		}
		device.destroySwapchainKHR(swapchain);
		device.destroy();
		instance.destroySurfaceKHR(surface);
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