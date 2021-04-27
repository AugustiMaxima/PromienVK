#include "../lv/helper/naive.hpp"
#include "../lv/helper/deviceLib.hpp"
#include "pipeline/boilerplate.hpp"
#include "pipeline/renderPass.hpp"
#include "pipeline/pipeline.hpp"
#include "pipeline/shader.hpp"
#include "harness.hpp"

namespace core {

	Harness::Harness(const std::string& config) {

		conf::parseConfigs(configs, config);

		auto& appScope = configs["Application"];
		auto display = processDisplaySettings(configs["Display"]);

		std::string title = (std::string&)appScope["Title"];

		lvl::configureInstance(instance, title, VK_MAKE_VERSION(appScope["MajorVersion"], appScope["MinorVersion"], appScope["PatchVersion"]));
		lvl::enumeratePhysicalDevices(instance);
		lvl::initializeSurface(instance, surface, display.resolution.width, display.resolution.height, title);

		pDevice = lvl::collectRenderingDevice(instance, surface);

		lv::PhysicalDevice& physicalDevice = *pDevice;
		
		//Device allocation logic is narly, even if you attempt to refactor
		std::map<infr::QueueFunction, std::function<bool(const vk::QueueFamilyProperties&)>> query;
		query[infr::QueueFunction::graphic] = lvl::graphicQueue;
		query[infr::QueueFunction::compute] = lvl::asyncComputeQueue;
		query[infr::QueueFunction::transfer] = lvl::transferQueue;
		
		auto queueIndice = lvl::getDeviceQueueIndex(physicalDevice, query);

		vk::DeviceCreateInfo graphic;

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		graphic.enabledExtensionCount = deviceExtensions.size();
		graphic.ppEnabledExtensionNames = deviceExtensions.data();

		std::vector<std::vector<float>> priorities;
		lvl::autoPopulatePriorities(physicalDevice, priorities);

		device = &lvl::allocateDevice(physicalDevice, graphic, priorities);

		graphicQueues = &(device->queues)[queueIndice[infr::QueueFunction::graphic]];
		auto transfix = queueIndice.find(infr::QueueFunction::transfer);
		if (transfix == queueIndice.end()) {
			transferQueues = graphicQueues;
		}
		else {
			transferQueues = &(device->queues)[transfix->second];
		}

		lvl::initializeSwapchain(swapchain, *device, surface, display);

		//pipeline configuration
		renderPass.device = device;
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

		renderPass.src = rend.construct(device->src);

		//
		pipeline::GraphicsPipelineEnclosure pip = pipeline::configureGraphicsPipeline(vk::PrimitiveTopology::eTriangleList, false, display.resolution);
		pip.shaders.entryName = { "main","main" };
		pip.shaders.srcs = { "shaders/shader.vert.spv", "shaders/shader.frag.spv" };
		pip.shaders.stages = { vk::ShaderStageFlagBits::eVertex , vk::ShaderStageFlagBits::eFragment };

		//configure your vertex assembly information here
		auto& vx = pip.vertice;
		vx.binding.push_back(vk::VertexInputBindingDescription()
			.setBinding(0)
			.setInputRate(vk::VertexInputRate::eVertex)
			.setStride(8 * sizeof(float)));
		vx.attribute.push_back(vk::VertexInputAttributeDescription()
			.setBinding(0)
			.setLocation(0)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(0));
		vx.attribute.push_back(vk::VertexInputAttributeDescription()
			.setBinding(0)
			.setLocation(2)
			.setFormat(vk::Format::eR32G32Sfloat)
			.setOffset(3 * sizeof(float)));
		vx.attribute.push_back(vk::VertexInputAttributeDescription()
			.setBinding(0)
			.setLocation(3)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setOffset(5 * sizeof(float)));


		renderPass.pipelines.push_back(lv::Pipeline());
		auto& pipeline = renderPass.pipelines.back();
		pipeline.pipeline = pip.construct(device->src, renderPass.src, 0, nullptr, -1, nullptr);
		pipeline.layout = pip.uniform.layout;

		for (auto& shader : pip.shaders.shaders) {
			device->src.destroy(shader);
		}
		
		//configuring present
		lvl::initializeFrameBuffer(frameBuffer, swapchain, renderPass, display);
		lvl::initializeFrameSynchronization(synchronization, frameBuffer, queueIndice[infr::QueueFunction::graphic]);

		//set up models (depends on transfer queue creation)


	}



}