#include "../lv/helper/naive.hpp"
#include "../lv/helper/deviceLib.hpp"
#include "pipeline/boilerplate.hpp"
#include "pipeline/renderPass.hpp"
#include "pipeline/pipeline.hpp"
#include "pipeline/shader.hpp"
#include "harness.hpp"
#include "../asset/io/streamWorks.hpp"

namespace core {

	Harness::Harness(const std::string& config) {
		conf::Scope configs;
		conf::parseConfigs(configs, config);

		conf::Scope appScope = configs["Application"];
		display = processDisplaySettings(configs["Display"]);

		std::string title = appScope["Title"];

		lvl::initWindow(surface, display.resolution.width, display.resolution.height, title);
		lvl::configureInstance(instance, title, VK_MAKE_VERSION(appScope["MajorVersion"], appScope["MinorVersion"], appScope["PatchVersion"]));
		lvl::enumeratePhysicalDevices(instance);
		lvl::initializeSurface(instance, surface);

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

		auto& gqInstance = (device->queues)[queueIndice[infr::QueueFunction::graphic]];
		auto transfix = queueIndice.find(infr::QueueFunction::transfer);
		if (transfix == queueIndice.end()) {
			//the code path we adopt gives at least one queue dedicated to transfer
			//so we do not have to use external synchronization
			//for this to work, we require either: separate transfer queues, or
			//more than one graphicQueues
			//this will not work for single queue devices
			//for that to work, we need to refactor to add explicit synchornization for all queue submission
			transferQueues.push_back(gqInstance.back().src);
			graphicQueues.resize(gqInstance.size() - 1);
			if (graphicQueues.size() == 0)
				throw std::runtime_error("Graphic Queues have insufficient number of queues for dedicated transfer ops");
			for (int i = 0; i < graphicQueues.size(); i++) {
				graphicQueues[i] = gqInstance[i].src;
			}
		}
		else {
			//transfer queue should be separate from graphics queue, otherwise this leads to synchronization problems
			auto& tqInstance = (device->queues)[transfix->second];
			graphicQueues.resize(gqInstance.size());
			transferQueues.resize(tqInstance.size());
			for (int i = 0; i < graphicQueues.size(); i++) {
				graphicQueues[i] = gqInstance[i].src;
			}
			for (int i = 0; i < transferQueues.size(); i++) {
				transferQueues[i] = tqInstance[i].src;
			}
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
		host.initialize(pDevice->src, device->src, queueIndice[infr::QueueFunction::transfer], transferQueues);

		lux = new asset::format::obj("assets/lux.obj");

		const int size = lux->getAttributes() * lux->getVerticeCount() * sizeof(float);
		asset::io::ModelToStage load{ host, *lux };
		load.start();
		asset::io::StageVueue stageBuffer = load.collectStage();
		vk::Buffer vr = device->src.createBuffer(vk::BufferCreateInfo()
			.setUsage(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setSize(size));
		asset::io::StagePropagation transfer{ host, vr, stageBuffer };
		transfer.start();
		device->src.waitForFences(transfer.getFence(), true, UINT64_MAX);
		vram = transfer.collectVRAM();
	}

	void Harness::start() {
		unsigned fc = 0;
		while (!glfwWindowShouldClose(surface.window)) {
			glfwPollEvents();
			renderFrame(fc++);
		}
	}

	void Harness::renderFrame(unsigned f) {
		vk::Device& rdevice = device->src;

		unsigned id = f % swapchain.images.size();
		unsigned il = f % synchronization.framesInFlight;

		if (synchronization.frameLease[il]) {
			rdevice.waitForFences(synchronization.frameLease[il], true, UINT64_MAX);
		}
		rdevice.resetCommandPool(synchronization.commandPools[il].src, vk::CommandPoolResetFlagBits::eReleaseResources);
		rdevice.waitForFences(synchronization.frameFinished[id], true, UINT64_MAX);

		rdevice.acquireNextImageKHR(swapchain.swapchain, UINT64_MAX, synchronization.imageAcquired[il], nullptr, &id);

		auto& cb = synchronization.commandPools[id].buffers[0].src;

		cb.begin(vk::CommandBufferBeginInfo());
		std::array<float, 4> cn = { 0,0,0,1 };
		vk::ClearValue clearColor = cn;
		cb.beginRenderPass(vk::RenderPassBeginInfo()
			.setClearValueCount(1)
			.setPClearValues(&clearColor)
			.setRenderPass(renderPass.src)
			.setRenderArea(vk::Rect2D()
				.setExtent(display.resolution)
				.setOffset({ 0,0 }))
			.setFramebuffer(synchronization.framebuffer->framebuffers[id])
			, vk::SubpassContents::eInline);
		cb.bindPipeline(vk::PipelineBindPoint::eGraphics, renderPass.pipelines[0].pipeline);
		cb.bindVertexBuffers((uint32_t)0, vram.buffer, (vk::DeviceSize)0);
		cb.draw(lux->getVerticeCount(), 1, 0, 0);
		cb.endRenderPass();
		cb.end();

		auto& gq = graphicQueues[f % graphicQueues.size()];


		vk::PipelineStageFlags wstage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		auto submit = vk::SubmitInfo()
			.setCommandBufferCount(1)
			.setPCommandBuffers(&synchronization.commandPools[id].buffers[0].src)
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(&synchronization.renderComplete[il])
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&synchronization.imageAcquired[il])
			.setPWaitDstStageMask(&wstage);

		rdevice.resetFences(synchronization.frameFinished[il]);
		gq.submit(submit, synchronization.frameFinished[il]);

		synchronization.frameLease[id] = synchronization.frameFinished[il];

		auto present = vk::PresentInfoKHR()
			.setSwapchainCount(1)
			.setPSwapchains(&swapchain.swapchain)
			.setPImageIndices(&id)
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&synchronization.renderComplete[il]);

		gq.presentKHR(present);

	}

	Harness::~Harness(){}

}